#include "generate_image.hpp"

int main() {
    Camera cam(1000, 1000, (d_t)M_PI / 2.f);

    std::string f_name = "data/point_cloud.ply";
    happly::PLYData ply_data(f_name);
    GaussianData data;

    std::vector<int> el(GaussianData::get_size(ply_data));
    std::iota(el.begin(), el.end(), 0);

    auto xyz = GaussianData::load_xyz(ply_data, el);
    auto [mn, mx] = GaussianData::range(xyz);
    cam.move_to((mn + mx) / (d_t)2.);

    data.load_data(ply_data, get_quad_block(0b110, 3, xyz));
    // GaussianData data = test();
    // cam.roll(M_PI/2);
    // cam.pan((d_t)M_PI / 2.f);
    cam.tilt(-(d_t)M_PI / 4.f);
    // cam.pan((d_t)M_PI);
    cam.tilt((d_t)M_PI / 8.f);
    cam.move_to(v4_t{0, 0, -1.5});
    DEBUG_PRINT("Step 1")
    Image front_image = render(cam, data);
    DEBUG_PRINT("Step 2")
    front_image.store_image("img.bmp");
    DEBUG_PRINT("Step 3")
    return 0;
}