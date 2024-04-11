#include "eigen/Eigen/Dense"
#include "happly.h"
#include <vector>
#include <iostream>
#include <cmath>

struct GaussianData{
    Eigen::MatrixXf xyz;
    Eigen::MatrixXf rot;
    Eigen::MatrixXf scale;
    Eigen::MatrixXf opacity;
    Eigen::MatrixXf sh;

    Eigen::MatrixXf flatten() {
        Eigen::MatrixXf ret(xyz.rows(), xyz.cols() + rot.cols() + scale.cols() + opacity.cols() + sh.cols());
        ret << xyz, rot, scale, opacity, sh;
        return ret;
    }

    size_t len() {
        return xyz.rows();
    }

    size_t sh_dim() {
        return sh.cols();
    }
};

GaussianData load_ply(const std::string &path){
    happly::PLYData data(path);

    // Load positions
    std::vector<float> x = data.getElement("vertex").getProperty<float>("x");
    std::vector<float> y = data.getElement("vertex").getProperty<float>("y");
    std::vector<float> z = data.getElement("vertex").getProperty<float>("z");

    Eigen::MatrixXf xyz(x.size(), 3);
    for (int i = 0; i < x.size(); i++){
        xyz(i, 0) = x[i];
        xyz(i, 1) = y[i];
        xyz(i, 2) = z[i];
    }

    // Load rotation
    Eigen::MatrixXf rot(x.size(), 4);
    std::vector<float> rot_x = data.getElement("vertex").getProperty<float>("rot_0");
    std::vector<float> rot_y = data.getElement("vertex").getProperty<float>("rot_1");
    std::vector<float> rot_z = data.getElement("vertex").getProperty<float>("rot_2");
    std::vector<float> rot_w = data.getElement("vertex").getProperty<float>("rot_3");
    for (int i = 0; i < rot_x.size(); ++i) {
        rot.row(i) << rot_x[i], rot_y[i], rot_z[i], rot_w[i];
    }

    // Load scale
    Eigen::MatrixXf scale(x.size(), 3);
    for (int i = 0; i < 3; i++) {
        std::vector<float> scale_i = data.getElement("vertex").getProperty<float>("scale_" + std::to_string(i));
        for (int j = 0; j < scale_i.size(); ++j) {
            scale(j, i) = scale_i[j];
        }
    }

    // Load opacity
    std::vector<float> opacities = data.getElement("vertex").getProperty<float>("opacity");
    Eigen::MatrixXf opacity(opacities.size(), 1);
    for (int i = 0; i < opacities.size(); i++){
        opacity(i, 0) = opacities[i];
    }

    // Load feature data
    int extra_features = 45;
    Eigen::MatrixXf features(x.size(), 3+extra_features); // Maybe some other shape here?
    features.col(0) = Eigen::Map<Eigen::VectorXf>(data.getElement("vertex").getProperty<float>("f_dc_0").data(), x.size());
    features.col(1) = Eigen::Map<Eigen::VectorXf>(data.getElement("vertex").getProperty<float>("f_dc_1").data(), x.size());
    features.col(2) = Eigen::Map<Eigen::VectorXf>(data.getElement("vertex").getProperty<float>("f_dc_2").data(), x.size());
    for (int i = 0; i < extra_features/3; i++) {
        for(int j = 0; j < 3; j++){
            int ind_feat = 3 + i*3 + j, ind_file = i + extra_features/3 * j;
            features.col(ind_feat) = Eigen::Map<Eigen::VectorXf>(data.getElement("vertex").getProperty<float>("f_rest_" + std::to_string(ind_file)).data(), x.size());
        }
    }

    // Normalizing rotations
    for (int i = 0; i < rot.rows(); ++i) {
        float norm = rot.row(i).norm();
        if (norm != 0) {
            rot.row(i) /= norm;
        }
    }

    // Apply exp to scale
    scale = scale.unaryExpr([](float val) { return std::exp(val); });

    // Apply sigmoid to opacities
    opacity = opacity.unaryExpr([](float val) { return 1.0f / (1.0f + std::exp(-val)); });

    return GaussianData{xyz, rot, scale, opacity, features};
}

int main() {
    GaussianData data = load_ply("data/point_cloud.ply");
    return 0; 
}