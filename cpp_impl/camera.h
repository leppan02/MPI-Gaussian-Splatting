#ifndef CAMERA_IMPORT
#define CAMERA_IMPORT

#include <math.h>

#include "default_types.h"
#include "vec.h"

namespace camera {

struct Camera {
    m_t mat, p_mat, complete_matrix;
    int image_size_x, image_size_y;
    d_t fov_x, px, py, f;
    Camera(int image_size_x, int image_size_y, d_t fov_x)
        : mat(v_t{1, 1, 1, 1}.diag()),
          image_size_x(image_size_x),
          image_size_y(image_size_y),
          fov_x(fov_x),
          px(image_size_x / 2.),
          py(image_size_y / 2.),
          f(px / tan(fov_x / 2)),
          // clang-format off
          p_mat({v_t{f, 0, px, 0},
                 v_t{0, f, py, 0},
                 v_t{0, 0, 1, 0},
                 v_t{0, 0, 0, 1}}) 
                 // clang-format on 
                 {}
    void update_matrix() {
        complete_matrix = mat_mul(p_mat, mat);
    }
    void pan(d_t deg_rad) {
        // Rotates around x axis
        d_t c = cos(deg_rad), s = sin(deg_rad);
        // clang-format off
        m_t rot_mat = {v_t{ 1, 0, 0, 0},
                       v_t{ 0, c,-s, 0},
                       v_t{ 0, s, c, 0},
                       v_t{ 0, 0, 0, 1}};
        // clang-format on
        mat = mat_mul(rot_mat, mat);
        update_matrix();
    }
    void tilt(d_t deg_rad) {
        // Rotates around y axis
        d_t c = cos(deg_rad), s = sin(deg_rad);
        // clang-format off
        m_t rot_mat = {v_t{ c, 0, s, 0},
                       v_t{ 0, 1, 0, 0},
                       v_t{-s, 0, c, 0},
                       v_t{ 0, 0, 0, 1}};
        // clang-format on
        mat = mat_mul(rot_mat, mat);
        update_matrix();
    }
    void roll(d_t deg_rad) {
        // Rotates around z axis
        d_t c = cos(deg_rad), s = sin(deg_rad);
        // clang-format off
        m_t rot_mat = {v_t{ c,-s, 0, 0},
                       v_t{ s, c, 0, 0},
                       v_t{ 0, 0, 1, 0},
                       v_t{ 0, 0, 0, 1}};
        // clang-format on
        mat = mat_mul(rot_mat, mat);
        update_matrix();
    }
    void move_to(v_t v) {
        // clang-format off
        m_t rot_mat = {v_t{ 1, 0, 0,-v[0]},
                       v_t{ 0, 1, 0,-v[1]},
                       v_t{ 0, 0, 1,-v[2]},
                       v_t{ 0, 0, 0,   1}};
        // clang-format on
        mat = mat_mul(rot_mat, mat);
        update_matrix();
    }
    // m_t cov2d(const m_t& cov3d, v_t camera_pos) {
    //     d_t x = camera_pos[0];
    //     d_t y = camera_pos[1];
    //     d_t z = camera_pos[2];
    //     // clang-format off
    //     m_t J{v_t{f/z, 0, f * x/(z*z)},
    //           v_t{0, f/z, 0, f * y/(z*z)},
    //           v_t{0,0,1,0},
    //           v_t{0,0,0,1}};
    //     // clang-format on
    //     return J;
    // }
};
};  // namespace camera
#endif