#include <chrono>

#include "generate_image.hpp"
#include "mpi.h"
#include "transpose_sort.cpp"

MPI_Comm comm = MPI_COMM_WORLD;
int world_size;
int world_rank;

/**
 * Sends an Image object to a specified destination using MPI_Send.
 *
 * @param image The Image object to be sent.
 * @param dest The destination rank to which the image will be sent.
 * @return 0 on success.
 */
int send_image(Image const &image, int dest) {
    MPI_Send(image.image.data(), image.image.size() * sizeof(image.image[0]),
             MPI_BYTE, dest, 0, comm);
    MPI_Send(image.alpha_mask.data(),
             image.alpha_mask.size() * sizeof(image.alpha_mask[0]), MPI_BYTE,
             dest, 0, comm);
    return 0;
}

/**
 * Receives the image and alpha mask data from the specified process.
 *
 * @param image The Image object to store the received image data.
 * @param orig The rank of the process from which to receive the data.
 * @return 0 on success, or an error code on failure.
 */
int recv_image(Image &image, int orig) {
    MPI_Status s;
    MPI_Recv(image.image.data(), image.image.size() * sizeof(image.image[0]),
             MPI_BYTE, orig, 0, MPI_COMM_WORLD, &s);
    MPI_Recv(image.alpha_mask.data(),
             image.alpha_mask.size() * sizeof(image.alpha_mask[0]), MPI_BYTE,
             orig, 0, MPI_COMM_WORLD, &s);
    return s.MPI_ERROR;
}

#define ts(var) auto var = std::chrono::high_resolution_clock::now()
#define diff(t1, t2) duration_cast<std::chrono::milliseconds>(t2 - t1).count()

/**
 * Retrieves a vector of tuples containing the dot product of each element's
 * position with the given direction vector and the corresponding element index.
 *
 * @param ply_data The PLYData object containing the element data.
 * @param dir The direction vector used for calculating the dot product.
 * @return A vector of tuples, where each tuple contains the dot product and the
 *         corresponding element index.
 */
vector<std::tuple<float, int>> get_elements(happly::PLYData &ply_data,
                                            v4_t dir) {
    int number_elements = GaussianData::get_size(ply_data);

    std::vector<int> el;
    for (int i = world_rank; i < number_elements; i += world_size) {
        el.push_back(i);
    }

    auto xyz = GaussianData::load_xyz(ply_data, el);
    std::vector<std::tuple<float, int>> data;
    for (size_t i = 0; i < xyz.size(); i++) {
        data.push_back({xyz[i].dot(dir), el[i]});
    }
    if (number_elements % world_size &&
        world_rank >= (number_elements % world_size)) {
        data.push_back({0, -1});
    }
    return data;
}

/**
 * Sorts the positions based on the depths.
 *
 * @param depths A vector of tuples containing the depths and indecies.
 * @return A vector of sorted indecies.
 */
vector<int> sort_positions(std::vector<std::tuple<float, int>> depths) {
    SortEngine<std::tuple<float, int>> sorter(depths, world_rank, world_size);
    sorter.run_sort();
    std::vector<int> elements;
    for (auto &d : sorter.mydata) {
        if (std::get<1>(d) != -1) {
            elements.push_back(std::get<1>(d));
        }
    }
    return elements;
}

/**
 * Combines images from different ranks in a parallel MPI environment.
 * 
 * This function takes an Image object and a Camera object as input parameters.
 * It combines images from different ranks in a parallel MPI environment using the following steps:
 * 1. It iterates over the ranks in a binary tree pattern, where each rank combines its image with the image of the rank at a distance of 'jump'.
 * 2. If the current rank is the receiver rank, it receives the image from the sender rank and combines it with its own image.
 * 3. If the current rank is the sender rank, it sends its image to the receiver rank.
 * 
 * @param image The Image object to combine the images into.
 * @param cam The Camera object used to capture the images.
 */
void combine_images(Image &image, Camera &cam) {
    for (int jump = 1; jump < world_size; jump *= 2) {
        if (world_rank % (jump * 2) == 0) {
            auto recv_rank = world_rank + jump;
            if (recv_rank >= world_size) continue;
            auto o_image = Image(cam);
            recv_image(o_image, recv_rank);
            image.combine(o_image);
        } else {
            auto send_rank = world_rank - jump;
            send_image(image, send_rank);
            break;
        }
    }
}

/**
 * @brief Runs the main MPI program.
 *
 * @param f_name The name of the PLY file to load.
 * @param barrier_comm The MPI communicator for barrier synchronization.
 * @return int Returns 0 upon successful execution.
 */
int run(std::string f_name, MPI_Comm barrier_comm) {
    MPI_Barrier(barrier_comm);
    ts(open_file);
    happly::PLYData ply_data(f_name);
    ts(done_open_file);
    Camera cam(1000, 1000, (d_t)M_PI / 2.f);

    cam.move_to({0.0706437, 1.88046, 1.16585, 1});
    cam.tilt(-(d_t)M_PI / 4.f);
    cam.tilt((d_t)M_PI / 8.f);
    cam.move_to(v4_t{0, 0, -1.5});

    GaussianData data;

    MPI_Barrier(barrier_comm);
    ts(load_xyz);
    auto depths = get_elements(ply_data, cam.r_mat4.mat_mul(v4_t{0, 0, 1, 1}));
    ts(done_load_xyz);

    MPI_Barrier(barrier_comm);
    ts(sort_xyz);
    auto el = sort_positions(depths);
    ts(done_sort_xyz);

    MPI_Barrier(barrier_comm);
    ts(load);
    data.load_data(ply_data, el);
    ts(done_load);

    MPI_Barrier(barrier_comm);
    ts(start_render);
    auto image = render(cam, data);
    ts(done_render);

    if (world_rank == 0) {
        DEBUG_PRINT("Data per process: " << data.xyz.size())
    }

    MPI_Barrier(barrier_comm);
    ts(comm);
    combine_images(image, cam);
    ts(done_comm);
    if (world_rank == 0) {
        image.add_background({1, 1, 1});
        image.store_image("img.bmp");
    }
    if (world_rank == 0) {
        DEBUG_PRINT("Processes: " << world_size)
        DEBUG_PRINT("Open file: " << diff(open_file, done_open_file) << "ms")
        DEBUG_PRINT("Load positions: " << diff(load_xyz, done_load_xyz) << "ms")
        DEBUG_PRINT("Sort positions: " << diff(sort_xyz, done_sort_xyz) << "ms")
        DEBUG_PRINT("Load: " << diff(load, done_load) << "ms")
        DEBUG_PRINT("Render: " << diff(start_render, done_render) << "ms")
        DEBUG_PRINT("Communication: " << diff(comm, done_comm) << "ms\n")
    }

    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &world_rank);

    auto ret = run("data/point_cloud.ply", MPI_COMM_WORLD);
    if (ret != 0) return ret;

    MPI_Finalize();
    return 0;
}

// *******************************
// FOR BENCHMARKING ON DARDEL
// *******************************
// int main(int argc, char **argv) {
//     MPI_Init(&argc, &argv);
//     MPI_Comm_size(comm, &world_size);
//     MPI_Comm_rank(comm, &world_rank);
//     int real_size = world_size;
//     for (world_size = 1; world_size <= real_size; world_size++) {
//         MPI_Comm barrier_comm;
//         if (world_rank < world_size) {
//             MPI_Comm_split(MPI_COMM_WORLD, 0, world_rank, &barrier_comm);
//             for (int i = 0; i < 5; i++) {
//                 auto ret = run("data/point_cloud.ply", barrier_comm);
//                 if (ret != 0) return ret;
//             }
//         } else {
//             MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, world_rank,
//                            &barrier_comm);
//         }
//         MPI_Barrier(MPI_COMM_WORLD);
//     }

//     MPI_Finalize();
//     return 0;
// }