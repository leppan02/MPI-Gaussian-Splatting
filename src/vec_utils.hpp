#ifndef VEC_UTILS_IMPORT
#define VEC_UTILS_IMPORT 1
#include "camera.hpp"
#include "default_types.hpp"
#include "vec.hpp"

template <typename T>
/**
 * Converts a quaternion to a 3x3 matrix.
 *
 * @param q The quaternion to convert.
 * @return The resulting 3x3 matrix.
 */
mat<T, 3, 3> quat_to_mat(const vec<T, 4>& q) {
    // clang-format off
    T q0=q[0],q1=q[1],q2=q[2],q3=q[3];
    return mat<T, 3, 3>{
        2*(q0*q0+q1*q1)-1,2*(q1*q2-q0*q3)  ,2*(q1*q3+q0*q2)  ,
        2*(q1*q2+q0*q3)  ,2*(q0*q0+q2*q2)-1,2*(q2*q3-q0*q1)  ,
        2*(q1*q3-q0*q2)  ,2*(q2*q3+q0*q1)  ,2*(q0*q0+q3*q3)-1};
    // clang-format on
}

template <typename T>
/**
 * Calculates the covariance matrix for a 3D point cloud.
 *
 * @param scale The scaling factors along the x, y, and z axes.
 * @param rot The rotation matrix representing the orientation.
 * @return The covariance matrix.
 */
mat<T, 3, 3> calc_cov3d(const vec<T, 3>& scale, const mat<T, 3, 3>& rot) {
    auto rot_T = rot.T();
    return rot_T.mat_mul_diag(scale.squared()).mat_mul_T(rot_T);
}

struct PlotData {
    float A, B, C, x_r, y_r, x_c, y_c;
    bool behind;
    template <typename T>
    /**
     * @brief Constructs a PlotData object.
     *
     * This constructor initializes a PlotData object with the given parameters.
     *
     * @param camera The camera object used for plotting.
     * @param g_pos_cam The position of the object in camera coordinates.
     * @param cov3d The 3x3 covariance matrix.
     */
    PlotData(const Camera& camera, const vec<T, 4>& g_pos_cam,
             const mat<T, 3, 3>& cov3d) {
        const T limx = 1.3f * camera.htanx * g_pos_cam[2],
                limy = 1.3f * camera.htany * g_pos_cam[2];
        const T x = max(min(g_pos_cam[0], limx), -limx),
                y = max(min(g_pos_cam[1], limy), -limy), z = g_pos_cam[2],
                z2 = z * z;
        const T fx = camera.f, fy = camera.f;
        // clang-format off
        mat<T, 3, 3> T_T = mat<T, 3, 3>
        {fx/z      ,0          ,0,
        0         ,fy/z       ,0,
        -(fx*x)/z2, -(fy*y)/z2,0}.mat_mul_T(camera.r_mat3_T);
        // clang-format on
        const auto cov = T_T.mat_mul_T(cov3d).mat_mul_T(T_T);
        // clang-format off
        mat<T, 2, 2>cov2d {(T)0.3+cov[0][0],     cov[0][1],
                                cov[1][0], (T)0.3+cov[1][1]};
        // clang-format on
        const auto det_inv =
            1 / (cov2d[0][0] * cov2d[1][1] - cov2d[1][0] * cov2d[0][1]);
        A = cov2d[1][1] * det_inv;
        B = -cov2d[0][1] * det_inv;
        C = cov2d[0][0] * det_inv;
        x_r = 3.0f * sqrt(cov2d[0][0]);
        y_r = 3.0f * sqrt(cov2d[1][1]);
        auto image_cord = camera.p_mat.mat_mul(g_pos_cam);
        x_c = image_cord[0] / image_cord[2];
        y_c = image_cord[1] / image_cord[2];
        behind = image_cord[2] <= 0;
    }
};

#endif