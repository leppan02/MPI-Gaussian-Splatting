#include "generate_image.hpp"
#include "mpi.h"
#include <chrono>

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

int run() {
    timestamp(start);
    Camera cam(1000, 1000, (d_t)M_PI / 2.f);
    std::string f_name = "data/point_cloud.ply";
    happly::PLYData ply_data(f_name);
    GaussianData data;

    std::vector<int> el(GaussianData::get_size(ply_data));
    std::iota(el.begin(), el.end(), 0);
    timestamp(openfile);
    auto xyz = GaussianData::load_xyz(ply_data, el);
    timestamp(loaded_xyz);

    v4_t mn = MAXFLOAT + v4_t{0}, mx = MAXFLOAT * -1 + v4_t{0};
    for (auto const &pos : xyz) {
        mn = min(mn, pos);
        mx = max(mx, pos);
    }
    mn[3] = mx[3] = 1;
    cam.move_to((mn + mx) / 2.f);
    cam.tilt(-(d_t)M_PI / 4.f);
    cam.tilt((d_t)M_PI / 8.f);
    cam.move_to(v4_t{0, 0, -1.5});

    Blocks qt(world_size, cam.r_mat4.mat_mul(v4_t{0, 0, 1, 1}), xyz);
    timestamp(sorting);
    auto s = qt.spans[world_rank];
    data.load_data(ply_data, s);
    timestamp(loaded);

    auto image = render(cam, data);
    if(world_rank == 0){
        DEBUG_PRINT("Data per process: " << data.xyz.size())
    }
    
    timestamp(rendered);
    for (int jump = 1; jump < world_size; jump *= 2) {
        if(world_rank % (jump * 2) == 0){
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
    if(world_rank == 0){
        image.add_background({1, 1, 1});
        image.store_image("img.bmp");
    }
    if(world_rank == 0){
        DEBUG_PRINT("Open file: "<<diff(start, openfile)<<"ms")
        DEBUG_PRINT("Load positions: "<<diff(openfile, loaded_xyz)<<"ms")
        DEBUG_PRINT("Sorting: "<<diff(loaded_xyz, sorting)<<"ms")
        DEBUG_PRINT("Loading: "<<diff(sorting, loaded)<<"ms")
        DEBUG_PRINT("Rendering: "<<diff(loaded, rendered)<<"ms")
        DEBUG_PRINT("Communication: "<<diff(rendered, comm_done)<<"ms\n")
    }  
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &world_rank);

    auto ret = run();
    if(ret!=0)return ret;

    MPI_Finalize();
    return 0;
}