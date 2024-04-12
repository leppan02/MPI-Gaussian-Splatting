#include <cmath>
#include <iostream>
#include <vector>

#include "cpp_impl2/camera.hpp"
#include "cpp_impl2/color.hpp"
#include "cpp_impl2/default_types.hpp"
#include "cpp_impl2/vec.hpp"
#include "cpp_impl2/vec_utils.hpp"
#include "eigen/Eigen/Dense"
#include "happly/happly.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(ARG) std::cerr<<ARG<<std::endl;
#else
#define DEBUG_PRINT(ARG) 
#endif
struct GaussianData {
    // Homogenous {x,y,z,1}
    std::vector<v4_t> xyz;
    std::vector<m3_t> cov3d;
    std::vector<ColorHarmonic> colors;
};

GaussianData load_ply(const std::string &path) {
    happly::PLYData data(path);
    GaussianData gaussian_data;
    // Load positions
    std::vector<float> x = data.getElement("vertex").getProperty<float>("x");
    std::vector<float> y = data.getElement("vertex").getProperty<float>("y");
    std::vector<float> z = data.getElement("vertex").getProperty<float>("z");

    for (size_t i = 0; i < x.size(); i++)
        gaussian_data.xyz.emplace_back(v4_t{x[i], y[i], z[i], 1});

    std::vector<float> rot_x =
        data.getElement("vertex").getProperty<float>("rot_0");
    std::vector<float> rot_y =
        data.getElement("vertex").getProperty<float>("rot_1");
    std::vector<float> rot_z =
        data.getElement("vertex").getProperty<float>("rot_2");
    std::vector<float> rot_w =
        data.getElement("vertex").getProperty<float>("rot_3");

    std::vector<float> scale_0 =
        data.getElement("vertex").getProperty<float>("scale_0");
    std::vector<float> scale_1 =
        data.getElement("vertex").getProperty<float>("scale_1");
    std::vector<float> scale_2 =
        data.getElement("vertex").getProperty<float>("scale_2");

    auto exp_ = [](float a) { return exp(a); };
    std::for_each(scale_0.begin(), scale_0.end(), exp_);
    std::for_each(scale_1.begin(), scale_1.end(), exp_);
    std::for_each(scale_2.begin(), scale_2.end(), exp_);

    // Load cov3d
    for (size_t i = 0; i < rot_x.size(); i++) {
        auto q = v4_t{rot_x[i], rot_y[i], rot_z[i], rot_w[i]}.normalized();
        m3_t rot_mat = quat_to_mat(q);
        gaussian_data.cov3d.emplace_back(
            cov3d(exp(v3_t{scale_0[i], scale_1[i], scale_2[i]}), rot_mat));
    }

    // Load opacity
    std::vector<float> opacity =
        data.getElement("vertex").getProperty<float>("opacity");

    // Load feature data
    size_t extra_features = 45;
    std::vector<std::vector<float>> features;  // Maybe some other shape here?
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_0"));
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_1"));
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_2"));
    for (size_t i = 0; i < extra_features / 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            size_t ind_file = i + extra_features / 3 * j;
            features.push_back(data.getElement("vertex").getProperty<float>(
                "f_rest_" + std::to_string(ind_file)));
        }
    }
    for (size_t ind = 0; ind < x.size(); ind++) {
        array<v3_t, 16> c;
        for (size_t i = 0; i < (3 + extra_features); i += 3) {
            c[i / 3] = v3_t{features[i][ind], features[i + 1][ind],
                            features[i + 2][ind]};
        }
        gaussian_data.colors.emplace_back(ColorHarmonic(
            std::move(c), 1.0f / (1.0f + std::exp(-opacity[ind]))));
    }
    return gaussian_data;
}
#include <numeric>
vector<int> sort_positions_in_direction(const vector<v4_t> &pos,
                                        const v4_t &dir) {
    vector<d_t> depth;
    for (const auto &v : pos) depth.emplace_back(v.dot(dir));

    vector<int> idx(pos.size());
    std::iota(idx.begin(), idx.end(), 0);
    stable_sort(idx.begin(), idx.end(),
                [&depth](int i1, int i2) { return depth[i1] < depth[i2]; });

    return idx;
}

void draw_gaussian(std::vector<std::vector<v3_t>> &image, const Camera &cam,
                   const v4_t &dir, v4_t xyz, m3_t cov3d,
                   ColorHarmonic color_h) {
    auto d = PlotData(cam, xyz, cov3d);
    auto color = color_h.get_color(dir);

    int start_x = max(0, (int)round(d.x_c - d.x_r));
    int start_y = max(0, (int)round(d.y_c - d.y_r));
    int end_x = min(cam.image_size_x-1, (int)round(d.x_c + d.x_r));
    int end_y = min(cam.image_size_y-1, (int)round(d.y_c + d.y_r));
    for (int x = start_x; x <= end_x; x++) {
        for (int y = start_y; y <= end_y; y++) {
            float c_x = x - d.x_c, c_y = y - d.y_c;
            float power =
                -(d.A * c_x * c_x + d.C * c_y * c_y) / 2.0f - d.B * c_x * c_y;
            float alpha = min(0.99f, color_h.opacity * exp(power));
            image[x][y] = color * alpha + image[x][y] * (1 - alpha);
        }
    }
}

auto render(const Camera &cam, const GaussianData &data) {
    std::vector<std::vector<v3_t>> image(
        cam.image_size_x, std::vector<v3_t>(cam.image_size_y, {1,1,1}));

    // Transform location
    vector<v4_t> trans_xyz(data.xyz.size());
    for (size_t di = 0; di < data.xyz.size(); di++)
        trans_xyz[di] = cam.r_mat4.mat_mul(data.xyz[di]);

    // Sort on depth
    const auto c_dir = v4_t{0, 0, -1, 0};
    vector<int> sort_ind = sort_positions_in_direction(trans_xyz, c_dir);

    v4_t camera_trans = cam.r_mat4_T[3];
    DEBUG_PRINT("Drawing")
    for (auto di : sort_ind) {
        draw_gaussian(image, cam, (data.xyz[di] - camera_trans).normalized(),
                      trans_xyz[di], data.cov3d[di], data.colors[di]);
    }
    return image;
}

void store_image(const std::vector<std::vector<v3_t>> &image, const std::string &file_name){
    int h = image.size(), w = image[0].size();
    std::ofstream file;
    file.open (file_name, std::ios::binary);
    for(int i = 0; i < h; i++)for(int j = 0; j < w; j++)for(int c = 0; c < 3; c++){
        file << (unsigned char)round(max(0.f, min(image[h-i-1][j][c] * 256.f, 255.f))); 
    }
    file.close();
}

int main() {
    Camera cam(4000, 4000, (d_t)M_PI / 2.f);
    cam.roll(M_PI/2);
    cam.tilt(-(d_t)M_PI / 6.f);
    cam.pan(-(d_t)M_PI / 4.f);
    cam.move_to(v4_t{-1.5f, 0, -2});
    GaussianData data = load_ply("data/point_cloud.ply");
    DEBUG_PRINT("Step 1")
    auto image = render(cam, data);
    DEBUG_PRINT("Step 2")
    store_image(image, "img.bmp");
    DEBUG_PRINT("Step 3")
    return 0;
}