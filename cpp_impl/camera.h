#ifndef CAMERA_IMPORT
#define CAMERA_IMPORT

#include <math.h>

#include "default_types.h"
#include "vec.h"

namespace camera {

struct Camera {
    m_t mat;
    int image_size_x = 256, image_size_y = 256;
    d_t fov_x = 90;
    Camera() : mat(v_t{1, 1, 1, 1}.diag()) {}
    m_t get_matrix() {
        d_t px = image_size_x / 2., py = image_size_y / 2.;
        d_t f = px / tan((M_PI / 180) * fov_x / 2);
        // clang-format off
        m_t p_mat = {v_t{ f, 0,px, 0},
                     v_t{ 0, f,py, 0},
                     v_t{ 0, 0, 1, 0},
                     v_t{ 0, 0, 0, 1}};
        // clang-format off
        return mat_mul(p_mat, mat);
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
    }
    void move_to(v_t v) {
        // clang-format off
        m_t rot_mat = {v_t{ 1, 0, 0,-v[0]},
                       v_t{ 0, 1, 0,-v[1]},
                       v_t{ 0, 0, 1,-v[2]},
                       v_t{ 0, 0, 0,   1}};
        // clang-format on
        mat = mat_mul(rot_mat, mat);
    }
};
};  // namespace camera
#endif