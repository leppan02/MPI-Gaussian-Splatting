#include <cmath>
#include <iostream>
#include <vector>

#include "cpp_impl/color.h"
#include "cpp_impl/default_types.h"
#include "cpp_impl/vec.h"
#include "eigen/Eigen/Dense"
#include "happly/happly.h"

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

    for (int i = 0; i < x.size(); i++)
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
    // Load cov3d
    for (int i = 0; i < rot_x.size(); i++) {
        v4_t rot = v4_t{rot_x[i], rot_y[i], rot_z[i], rot_w[i]};
        m3_t rot_mat = vec::quat_to_mat(rot / sqrt(rot.norm()));
        gaussian_data.cov3d.emplace_back(vec::cov3d(exp(v3_t{scale_0[i], scale_1[i], scale_2[i]}), rot_mat));
    }

    // Load opacity
    std::vector<float> opacity =
        data.getElement("vertex").getProperty<float>("opacity");

    // Load feature data
    int extra_features = 45;
    std::vector<std::vector<float>> features;  // Maybe some other shape here?
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_0"));
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_1"));
    features.push_back(data.getElement("vertex").getProperty<float>("f_dc_2"));
    for (int i = 0; i < extra_features / 3; i++) {
        for (int j = 0; j < 3; j++) {
            int ind_feat = 3 + i * 3 + j, ind_file = i + extra_features / 3 * j;
            features.push_back(data.getElement("vertex").getProperty<float>(
                "f_rest_" + std::to_string(ind_file)));
        }
    }
    for (int ind = 0; ind < x.size(); ind++) {
        vec::vec<v3_t, 16> c;
        for (int i = 0; i < (3 + extra_features); i += 3) {
            c[i / 3] = v3_t{features[i][ind], features[i + 1][ind],
                           features[i + 2][ind]};
        }
        gaussian_data.colors.emplace_back(
            ColorHarmonic(c, 1.0f / (1.0f + std::exp(-opacity[ind]))));
    }
    std::cout<<gaussian_data.xyz.size()<<std::endl;
    std::cout<<gaussian_data.xyz.size() * (sizeof(v4_t) + sizeof(ColorHarmonic) + sizeof(float)) / 1000 / 1000 <<std::endl;
    return gaussian_data;
}

int main() {
    GaussianData data = load_ply("data/point_cloud.ply");
    return 0;
}