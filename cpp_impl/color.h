#ifndef COLOR_IMPORT
#define COLOR_IMPORT

#include "default_types.h"
#include "vec.h"

struct ColorHarmonic {
    vec::vec<v_t, 16> sh;
    ColorHarmonic(vec::vec<v_t, 16> sh) : sh(sh) {
        sh[0] = sh[0] * (d_t) 0.28209479177387814;

        sh[1] = sh[1] * (d_t) 0.4886025119029199;
        sh[2] = sh[2] * (d_t) 0.4886025119029199;
        sh[3] = sh[3] * (d_t) 0.4886025119029199;

        sh[4] = sh[4] * (d_t) 1.0925484305920792;
        sh[5] = sh[5] * (d_t) -1.0925484305920792;
        sh[6] = sh[6] * (d_t) 0.31539156525252005;
        sh[7] = sh[7] * (d_t) -1.0925484305920792;
        sh[8] = sh[8] * (d_t) 0.5462742152960396;

        sh[9] = sh[9] * (d_t) -0.5900435899266435;
        sh[10] = sh[10] * (d_t) 2.890611442640554;
        sh[11] = sh[11] * (d_t) -0.4570457994644658;
        sh[12] = sh[12] * (d_t) 0.3731763325901154;
        sh[13] = sh[13] * (d_t) -0.4570457994644658;
        sh[14] = sh[14] * (d_t) 1.445305721320277;
        sh[15] = sh[15] * (d_t) -0.5900435899266435;
    }

    v_t get_color(v_t dir) {
        v_t color = sh[0];

        color = color - sh[1] * dir[1];
        color = color - sh[2] * dir[2];
        color = color - sh[3] * dir[0];

        color = color + sh[4] * dir[0] * dir[1];
        color = color + sh[5] * dir[1] * dir[2];
        color = color + sh[6] * ((d_t)2.0 * dir[2] * dir[2] - dir[0] * dir[0] -
                                 dir[1] * dir[1]);
        color = color + sh[7] * dir[0] * dir[2];
        color = color + sh[8] * (dir[0] * dir[0] - dir[1] * dir[1]);

        color = color +
                sh[9] * dir[1] * ((d_t)3.0 * dir[0] * dir[0] - dir[1] * dir[1]);
        color = color + sh[10] * dir[0] * dir[1] * dir[2];
        color = color + sh[11] * dir[1] *
                            ((d_t)4.0 * dir[2] * dir[2] - dir[0] * dir[0] -
                             dir[1] * dir[1]);
        color = color +
                sh[12] * dir[2] *
                    ((d_t)2.0 * dir[2] * dir[2] - (d_t)3.0 * dir[0] * dir[0] -
                     (d_t)3.0 * dir[1] * dir[1]);
        color = color + sh[13] * dir[0] *
                            ((d_t)4.0 * dir[2] * dir[2] - dir[0] * dir[0] -
                             dir[1] * dir[1]);
        color = color + sh[14] * dir[2] * (dir[0] * dir[0] - dir[1] * dir[1]);
        color = color + sh[15] * dir[0] *
                            (dir[0] * dir[0] - (d_t)3.0 * dir[1] * dir[1]);

        color = color + (d_t)0.5;
        color = min(max(color, (d_t)0.), (d_t)1.);
        return color;
    }
};

#endif