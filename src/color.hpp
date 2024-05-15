#ifndef COLOR_IMPORT
#define COLOR_IMPORT 1

#include "default_types.hpp"
#include "vec.hpp"

/**
 * @brief Represents a color defined by spherical harmonics coefficients and opacity.
 */
struct ColorHarmonic {
    array<v3_t, 16> sh; /**< The spherical harmonics coefficients. */
    d_t opacity; /**< The opacity of the color. */

    /**
     * @brief Constructs a ColorHarmonic object with the given spherical harmonics coefficients and opacity.
     * 
     * @param sh_ The spherical harmonics coefficients.
     * @param opacity The opacity of the color.
     */
    ColorHarmonic(array<v3_t, 16> sh_, d_t opacity)
        : sh(std::move(sh_)), opacity(opacity) {
        sh[0] = sh[0] * (d_t)0.28209479177387814;
        sh[1] = sh[1] * (d_t)0.4886025119029199;
        sh[2] = sh[2] * (d_t)0.4886025119029199;
        sh[3] = sh[3] * (d_t)0.4886025119029199;
        sh[4] = sh[4] * (d_t)1.0925484305920792;
        sh[5] = sh[5] * (d_t)-1.0925484305920792;
        sh[6] = sh[6] * (d_t)0.31539156525252005;
        sh[7] = sh[7] * (d_t)-1.0925484305920792;
        sh[8] = sh[8] * (d_t)0.5462742152960396;
        sh[9] = sh[9] * (d_t)-0.5900435899266435;
        sh[10] = sh[10] * (d_t)2.890611442640554;
        sh[11] = sh[11] * (d_t)-0.4570457994644658;
        sh[12] = sh[12] * (d_t)0.3731763325901154;
        sh[13] = sh[13] * (d_t)-0.4570457994644658;
        sh[14] = sh[14] * (d_t)1.445305721320277;
        sh[15] = sh[15] * (d_t)-0.5900435899266435;
    }

    /**
     * @brief Calculates the color based on the direction vector.
     * 
     * @param dir The direction vector.
     * @return The calculated color.
     */
    v3_t get_color(v4_t dir) {
        v3_t color = sh[0];
        color = color - sh[1] * dir[1];
        color = color + sh[2] * dir[2];
        color = color - sh[3] * dir[0];
        // color = color + sh[4] * dir[0] * dir[1];
        // color = color + sh[5] * dir[1] * dir[2];
        // color = color + sh[6] * ((d_t)2.0 * dir[2] * dir[2] - dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[7] * dir[0] * dir[2];
        // color = color + sh[8] * (dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[9] * dir[1] * ((d_t)3.0 * dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[10] * dir[0] * dir[1] * dir[2];
        // color = color + sh[11] * dir[1] * ((d_t)4.0 * dir[2] * dir[2] - dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[12] * dir[2] * ((d_t)2.0 * dir[2] * dir[2] - (d_t)3.0 * dir[0] * dir[0] - (d_t)3.0 * dir[1] * dir[1]);
        // color = color + sh[13] * dir[0] * ((d_t)4.0 * dir[2] * dir[2] - dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[14] * dir[2] * (dir[0] * dir[0] - dir[1] * dir[1]);
        // color = color + sh[15] * dir[0] * (dir[0] * dir[0] - (d_t)3.0 * dir[1] * dir[1]);
        color = color + (d_t)0.5;
        color[0] = min(max(color[0], (d_t)0.), (d_t)1.);
        color[1] = min(max(color[1], (d_t)0.), (d_t)1.);
        color[2] = min(max(color[2], (d_t)0.), (d_t)1.);
        return color;
    }
};

#endif