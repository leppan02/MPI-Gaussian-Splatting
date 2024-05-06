#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <span>
#include <unordered_map>
#include <vector>

#include "include.hpp"
#include "../happly/happly.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(ARG) std::cerr << ARG << std::endl;
#else
#define DEBUG_PRINT(ARG)
#endif

/**
 * @brief Struct representing Gaussian data.
 */
struct GaussianData {
    std::vector<v4_t> xyz;               ///< Vector of 4D positions
    std::vector<m3_t> cov3d;             ///< Vector of 3x3 covariance matrices
    std::vector<ColorHarmonic> colors;   ///< Vector of color harmonics

    /**
     * @brief Get the size of the Gaussian data.
     * @param ply_data The PLY data object.
     * @return The size of the Gaussian data.
     */
    static size_t get_size(happly::PLYData &ply_data) {
        return ply_data.getElement("vertex").count;
    }

    /**
     * @brief Load the XYZ positions from the PLY data.
     * @param ply_data The PLY data object.
     * @param el The indices of the elements to load.
     * @return The vector of XYZ positions.
     */
    static std::vector<v4_t> load_xyz(happly::PLYData &ply_data,
                                      const vector<int> &el) {
        std::vector<d_t> x =
            ply_data.getElement("vertex").getProperty<float>("x");
        std::vector<d_t> y =
            ply_data.getElement("vertex").getProperty<float>("y");
        std::vector<d_t> z =
            ply_data.getElement("vertex").getProperty<float>("z");

        std::vector<v4_t> result;

        for (auto i : el) {
            result.emplace_back(v4_t{x[i], y[i], z[i], 1});
        }
        return result;
    }

    /**
     * @brief Load the 3D covariance matrices from the PLY data.
     * @param ply_data The PLY data object.
     * @param el The indices of the elements to load.
     * @return The vector of 3D covariance matrices.
     */
    static std::vector<m3_t> load_cov3d(happly::PLYData &ply_data,
                                        const vector<int> &el) {
        std::vector<d_t> rot_x =
            ply_data.getElement("vertex").getProperty<float>("rot_0");
        std::vector<d_t> rot_y =
            ply_data.getElement("vertex").getProperty<float>("rot_1");
        std::vector<d_t> rot_z =
            ply_data.getElement("vertex").getProperty<float>("rot_2");
        std::vector<d_t> rot_w =
            ply_data.getElement("vertex").getProperty<float>("rot_3");

        std::vector<d_t> scale_0 =
            ply_data.getElement("vertex").getProperty<float>("scale_0");
        std::vector<d_t> scale_1 =
            ply_data.getElement("vertex").getProperty<float>("scale_1");
        std::vector<d_t> scale_2 =
            ply_data.getElement("vertex").getProperty<float>("scale_2");

        std::vector<m3_t> result;

        for (auto i : el) {
            m3_t rot_mat = quat_to_mat(
                v4_t{rot_x[i], rot_y[i], rot_z[i], rot_w[i]}.normalized());
            result.emplace_back(calc_cov3d(
                exp(v3_t{scale_0[i], scale_1[i], scale_2[i]}), rot_mat));
        }
        return result;
    }

    /**
     * @brief Load the colors from the PLY data.
     * @param ply_data The PLY data object.
     * @param el The indices of the elements to load.
     * @return The vector of color harmonics.
     */
    static std::vector<ColorHarmonic> load_colors(happly::PLYData &ply_data,
                                                  const vector<int> &el) {
        // Load opacity
        std::vector<d_t> opacity =
            ply_data.getElement("vertex").getProperty<float>("opacity");

        // Load feature data
        size_t extra_features = 45;
        std::vector<std::vector<d_t>> features;  // Maybe some other shape here?
        features.push_back(
            ply_data.getElement("vertex").getProperty<float>("f_dc_0"));
        features.push_back(
            ply_data.getElement("vertex").getProperty<float>("f_dc_1"));
        features.push_back(
            ply_data.getElement("vertex").getProperty<float>("f_dc_2"));
        for (size_t i = 0; i < extra_features / 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                size_t ind_file = i + extra_features / 3 * j;
                features.push_back(
                    ply_data.getElement("vertex").getProperty<float>(
                        "f_rest_" + std::to_string(ind_file)));
            }
        }

        std::vector<ColorHarmonic> result;
        for (auto i : el) {
            array<v3_t, 16> c;
            for (size_t f_i = 0; f_i < (3 + extra_features); f_i += 3) {
                c[f_i / 3] = v3_t{features[f_i][i], features[f_i + 1][i],
                                  features[f_i + 2][i]};
            }
            result.emplace_back(ColorHarmonic(
                std::move(c), 1.0f / (1.0f + std::exp(-opacity[i]))));
        }
        return result;
    }

    /**
     * @brief Load the Gaussian data from the PLY data.
     * @param ply_data The PLY data object.
     * @param el The indices of the elements to load.
     */
    void load_data(happly::PLYData &ply_data, const vector<int> &el) {
        xyz = load_xyz(ply_data, el);
        cov3d = load_cov3d(ply_data, el);
        colors = load_colors(ply_data, el);
    }

    /**
     * @brief Load test data for the Gaussian data.
     */
    void load_test() {
        xyz = {v4_t{0, 0, 0, 1}, v4_t{1, 0, 0, 1}, v4_t{0, 1, 0, 1},
               v4_t{0, 0, -1, 1}};
        cov3d = {
            calc_cov3d(v3_t{0.03, 0.03, 0.03}, quat_to_mat(v4_t{1, 0, 0, 0})),
            calc_cov3d(v3_t{0.2, 0.03, 0.03}, quat_to_mat(v4_t{1, 0, 0, 0})),
            calc_cov3d(v3_t{0.03, 0.2, 0.03}, quat_to_mat(v4_t{1, 0, 0, 0})),
            calc_cov3d(v3_t{0.03, 0.03, 0.2}, quat_to_mat(v4_t{1, 0, 0, 0}))};
        colors = {ColorHarmonic(array<v3_t, 16>{1, 0, 1}, 1.f),
                  ColorHarmonic(array<v3_t, 16>{1, 0, 0}, 1.f),
                  ColorHarmonic(array<v3_t, 16>{0, 1, 0}, 1.f),
                  ColorHarmonic(array<v3_t, 16>{0, 1, 1}, 1.f)};
    }

    /**
     * @brief Get the range of XYZ positions.
     * @param xyz The vector of XYZ positions.
     * @return The range of XYZ positions.
     */
    static std::pair<v4_t, v4_t> range(std::vector<v4_t> const &xyz) {
        std::pair<v4_t, v4_t> res = {MAXFLOAT + v4_t{0},
                                     MAXFLOAT * -1 + v4_t{0}};
        for (const auto &pos : xyz) {
            res.first = min(pos, res.first);
            res.second = max(pos, res.second);
        }
        return res;
    }
};

/**
 * Sorts the indices of a span of positions in the given direction.
 *
 * This function sorts the indices of a span of positions based on their
 * projection onto the given direction vector. The sorting is done in ascending
 * order.
 *
 * @param pos The vector of positions.
 * @param dir The direction vector.
 * @param idx The span of indices to be sorted.
 */
void sort_span_in_direction(const vector<v4_t> &pos, const v4_t &dir,
                            std::span<int> idx) {
    std::unordered_map<int, d_t> depth;
    for (auto i : idx) depth[i] = (pos[i].dot(dir));
    stable_sort(idx.begin(), idx.end(),
                [&depth](int i1, int i2) { return depth[i1] < depth[i2]; });
}

/**
 * Retrieves a quad block based on the given ID, depth, and position vector.
 *
 * @param id The ID of the quad block.
 * @param depth The depth of the quad block.
 * @param pos The position vector containing v4_t elements.
 * @return A vector of integers representing the quad block.
 */
vector<int> get_quad_block(int id, int depth, const vector<v4_t> &pos) {
    int l = 0, r = pos.size();
    v4_t dirs[3] = {v4_t{1, 0, 0, 0}, v4_t{0, 1, 0, 0}, v4_t{0, 0, 1, 0}};
    vector<int> idx(pos.size());
    std::iota(idx.begin(), idx.end(), 0);
    for (int i = 0; i < depth; i++) {
        std::span<int> view(idx.begin() + l, idx.begin() + r);
        sort_span_in_direction(pos, dirs[i % 3], view);
        if (id & 1)
            l = (l + r) / 2;
        else
            r = (l + r) / 2;
        id >>= 1;
    }
    return std::vector<int>(idx.begin() + l, idx.begin() + r);
}

/**
 * Sorts the positions in the given vector `pos` in the direction specified by
 * the vector `dir`.
 *
 * @param pos The vector of positions to be sorted.
 * @param dir The direction vector along which the positions should be sorted.
 * @return A vector of integers representing the sorted positions.
 */
vector<int> sort_positions_in_direction(const vector<v4_t> &pos,
                                        const v4_t &dir) {
    vector<int> idx(pos.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::span<int> view(idx);
    sort_span_in_direction(pos, dir, view);
    return idx;
}

/**
 * @brief Represents an image with pixel values and an alpha mask.
 */
struct Image {
    int w, h;                             /**< Width and height of the image. */
    std::vector<std::vector<v3_t>> image; /**< Pixel values of the image. */
    std::vector<std::vector<float>> alpha_mask; /**< Alpha mask of the image. */

    /**
     * @brief Constructs an Image object with the given camera.
     *
     * @param cam The camera object used to determine the image size.
     */
    Image(Camera const &cam) : w(cam.image_size_x), h(cam.image_size_y) {
        image =
            std::vector<std::vector<v3_t>>(h, std::vector<v3_t>(w, {1, 1, 1}));
        alpha_mask =
            std::vector<std::vector<float>>(h, std::vector<float>(w, 1));
    }

    /**
     * @brief Combines the current image with another image.
     *
     * The pixel values of the current image are blended with the pixel values
     * of the provided image based on the alpha mask.
     *
     * @param behind The image to be combined with the current image.
     */
    void combine(Image const &behind) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                image[y][x] = image[y][x] * (1 - alpha_mask[y][x]) +
                              behind.image[y][x] * alpha_mask[y][x];
            }
        }
    }
    /**
     * Stores the image with the given file name.
     *
     * @param file_name The name of the file to store the image.
     */
    void store_image(const std::string &file_name) const {
        std::ofstream file;
        file.open(file_name, std::ios::binary);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                for (int c = 0; c < 3; c++) {
                    file << (unsigned char)max(
                        0, min((int)floor(image[i][j][c] * 256.f), 0xff));
                }
        file.close();
    }
};

/**
 * Draws a Gaussian splat on the given image using the specified camera, direction, position, covariance, and color.
 *
 * @param image The image on which to draw the Gaussian splat.
 * @param cam The camera used to capture the image.
 * @param dir The direction of the Gaussian splat.
 * @param xyz The position of the Gaussian splat.
 * @param cov3d The covariance matrix of the Gaussian splat.
 * @param color_h The color harmonic used to determine the color of the Gaussian splat.
 */
void draw_gaussian(Image &image, const Camera &cam, const v4_t &dir, v4_t xyz,
                   m3_t cov3d, ColorHarmonic color_h) {
    auto d = PlotData(cam, xyz, cov3d);

    if (d.behind) return;
    auto color = color_h.get_color(dir);

    int start_x = max(0, (int)round(d.x_c - d.x_r));
    int start_y = max(0, (int)round(d.y_c - d.y_r));
    int end_x = min(cam.image_size_x, (int)round(d.x_c + d.x_r) + 1);
    int end_y = min(cam.image_size_y, (int)round(d.y_c + d.y_r) + 1);
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            float c_x = x - d.x_c, c_y = y - d.y_c;
            float power =
                -(d.A * c_x * c_x + d.C * c_y * c_y) / 2.0f - d.B * c_x * c_y;
            float alpha = min(0.99f, color_h.opacity * exp(power));
            image.image[y][x] = color * alpha + image.image[y][x] * (1 - alpha);
            image.alpha_mask[y][x] *= (1 - alpha);
        }
    }
}

/**
 * Renders the scene using the given camera and Gaussian data.
 *
 * @param cam The camera object used for rendering.
 * @param data The Gaussian data containing the scene information.
 * @return The rendered image.
 */
auto render(const Camera &cam, const GaussianData &data) {
    Image image(cam);

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