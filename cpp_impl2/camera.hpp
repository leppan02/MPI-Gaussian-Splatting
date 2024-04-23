#ifndef CAMERA_IMPORT
#define CAMERA_IMPORT 1

#include <math.h>

#include "default_types.hpp"
#include "vec.hpp"

struct Camera {
    m4_t r_mat4, r_mat4_T, p_mat;
    m3_t r_mat3, r_mat3_T;
    int image_size_x, image_size_y;
    d_t fov_x, px, py, f, htanx, htany;
    Camera(int image_size_x, int image_size_y, d_t fov_x)
        : r_mat4(diag(v4_t{1, 1, 1, 1})),
          image_size_x(image_size_x),
          image_size_y(image_size_y),
          fov_x(fov_x) {
        px = (d_t)image_size_x / 2.f;
        py = (d_t)image_size_y / 2.f;
        htanx = tan(fov_x / 2);
        htany = htanx / px * py;
        f = px / htanx;

        // clang-format off
        p_mat = m4_t{f, 0, px, 0,
                     0, f, py, 0,
                     0, 0,  1, 0,
                     0, 0,  0, 1};
        // clang-format on
        update_matrix();
    }
    void update_matrix() {
        // Inexpensive caching
        // clang-format off
        r_mat3 = m3_t{r_mat4[0][0], r_mat4[0][1], r_mat4[0][2],
                      r_mat4[1][0], r_mat4[1][1], r_mat4[1][2],
                      r_mat4[2][0], r_mat4[2][1], r_mat4[2][2]};
        // clang-format on 
        r_mat4_T = r_mat4.T();
        r_mat3_T = r_mat3.T();
    }
    void tilt(d_t rad) {
        // Rotates around x axis
        d_t c = cos(rad), s = sin(rad);
        // clang-format off
        m4_t rot_mat = {1, 0, 0, 0,
                        0, c,-s, 0,
                        0, s, c, 0,
                        0, 0, 0, 1};
        // clang-format on
        r_mat4 = rot_mat.mat_mul(r_mat4);
        update_matrix();
    }
    void pan(d_t rad) {
        // Rotates around y axis
        d_t c = cos(rad), s = sin(rad);
        // clang-format off
        m4_t rot_mat = { c, 0, s, 0,
                         0, 1, 0, 0,
                        -s, 0, c, 0,
                         0, 0, 0, 1};
        // clang-format on
        r_mat4 = rot_mat.mat_mul(r_mat4);
        update_matrix();
    }
    void roll(d_t rad) {
        // Rotates around z axis
        d_t c = cos(rad), s = sin(rad);
        // clang-format off
        m4_t rot_mat = { c,-s, 0, 0,
                         s, c, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1};
        // clang-format on
        r_mat4 = rot_mat.mat_mul(r_mat4);
        update_matrix();
    }
    void move_to(const v3_t &v) {
        // clang-format off
        m4_t rot_mat = { 1, 0, 0,-v[0],
                         0, 1, 0,-v[1],
                         0, 0, 1,-v[2],
                         0, 0, 0,   1};
        // clang-format on
        r_mat4 = rot_mat.mat_mul(r_mat4);
        update_matrix();
    }
    void move_to(const v4_t &v) {
        // clang-format off
        m4_t rot_mat = { 1, 0, 0,-v[0],
                         0, 1, 0,-v[1],
                         0, 0, 1,-v[2],
                         0, 0, 0,   1};
        // clang-format on
        r_mat4 = rot_mat.mat_mul(r_mat4);
        update_matrix();
    }
};
#endif