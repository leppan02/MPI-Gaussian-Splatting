#include <chrono>

#include "generate_image.hpp"
#include "mpi.h"
#include "transpose_sort.cpp"

MPI_Comm comm = MPI_COMM_WORLD;
int world_size;
int world_rank;

int send_image(Image const &image, int dest) {
    int w = image.w;
    int h = image.h;
    MPI_Request color_request;
    MPI_Request alpha_request;
    MPI_Isend(image.image.data(), image.image.size() * sizeof(image.image[0]),
              MPI_BYTE, dest, 0, comm, &color_request);
    MPI_Isend(image.alpha_mask.data(),
              image.alpha_mask.size() * sizeof(image.alpha_mask[0]), MPI_BYTE,
              dest, 0, comm, &alpha_request);
    if (MPI_Wait(&color_request, MPI_STATUS_IGNORE) != 0) return -1;
    if (MPI_Wait(&alpha_request, MPI_STATUS_IGNORE) != 0) return -1;
    return 0;
}

int recv_image(Image &image, int orig) {
    int w = image.w;
    int h = image.h;
    MPI_Request color_request;
    MPI_Request alpha_request;
    MPI_Irecv(image.image.data(), image.image.size() * sizeof(image.image[0]),
              MPI_BYTE, orig, 0, comm, &color_request);
    MPI_Irecv(image.alpha_mask.data(),
              image.alpha_mask.size() * sizeof(image.alpha_mask[0]), MPI_BYTE,
              orig, 0, comm, &alpha_request);
    if (MPI_Wait(&color_request, MPI_STATUS_IGNORE) != 0) return -1;
    if (MPI_Wait(&alpha_request, MPI_STATUS_IGNORE) != 0) return -1;
    return 0;
}

#define timestamp(var) auto var = std::chrono::high_resolution_clock::now()
#define diff(t1, t2) duration_cast<std::chrono::milliseconds>(t2 - t1).count()

vector<int> get_elements(happly::PLYData &ply_data, v4_t dir) {
    int number_elements = GaussianData::get_size(ply_data);

    std::vector<int> el;
    for (int i = world_rank; i < number_elements; i += world_size) {
        el.push_back(i);
    }

    auto xyz = GaussianData::load_xyz(ply_data, el);
    std::vector<std::tuple<float, int>> data;
    for (int i = 0; i < xyz.size(); i++) {
        data.push_back({xyz[i].dot(dir), el[i]});
    }
    if (number_elements % world_size &&
        world_rank >= (number_elements % world_size)) {
        data.push_back({0, -1});
    }
    SortEngine<std::tuple<float, int>> sorter(data, world_rank, world_size);
    sorter.run_sort();
    std::vector<int> elements;
    for (auto &d : sorter.mydata) {
        if (std::get<1>(d) != -1) {
            elements.push_back(std::get<1>(d));
        }
    }
    return elements;
}

int run(std::string f_name) {
    timestamp(start);

    happly::PLYData ply_data(f_name);

    Camera cam(1000, 1000, (d_t)M_PI / 2.f);

    cam.move_to({0.0706437, 1.88046, 1.16585, 1});
    cam.tilt(-(d_t)M_PI / 4.f);
    cam.tilt((d_t)M_PI / 8.f);
    cam.move_to(v4_t{0, 0, -1.5});
    
    GaussianData data;

    timestamp(openfile);
    timestamp(loaded_xyz);
    auto el = get_elements(ply_data, cam.r_mat4.mat_mul(v4_t{0, 0, 1, 1}));
    data.load_data(ply_data, el);
    timestamp(loaded);

    auto image = render(cam, data);
    if (world_rank == 0) {
        DEBUG_PRINT("Data per process: " << data.xyz.size())
    }

    timestamp(rendered);
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
    timestamp(comm_done);
    if (world_rank == 0) {
        image.add_background({1, 1, 1});
        image.store_image("img.bmp");
    }
    if (world_rank == 0) {
        DEBUG_PRINT("Processes: " << world_size)
        DEBUG_PRINT("Open file: " << diff(start, openfile) << "ms")
        DEBUG_PRINT("Load positions: " << diff(openfile, loaded_xyz) << "ms")
        DEBUG_PRINT("Rendering: " << diff(loaded, rendered) << "ms")
        DEBUG_PRINT("Communication: " << diff(rendered, comm_done) << "ms\n")
    }
    //     break;
    // }
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &world_rank);
    int real_size = world_size;
    for (world_size = real_size; world_size <= real_size; world_size++) {
        if (world_rank < world_size) {
            for (int i = 0; i < 1; i++) {
                auto ret = run("data/point_cloud.ply");
                if (ret != 0) return ret;
            }
        }
    }

    MPI_Finalize();
    return 0;
}