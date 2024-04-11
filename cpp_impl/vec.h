#ifndef VEC_IMPORT
#define VEC_IMPORT

#include <math.h>

#include <array>
#include <iostream>
#include <ostream>

namespace vec {
using std::array;

template <class T, int D>
struct vec : public array<T, D> {
    typedef vec<T, D> _v;
    typedef vec<_v, D> _m;
    T sum() const {
        T out = 0;
        for (int i = 0; i < D; i++) out += (*this)[i];
        return out;
    }
    T dot(const _v& o) const { return ((*this) * o).sum(); }
    _v squared() const { return (*this) * (*this); }
    T norm() const { return (*this).squared().sum(); }

    _m diag() const {
        _m out{};
        for (int i = 0; i < D; i++) out[i][i] = (*this)[i];
        return out;
    }
};

#define IMPL_ops(op)                                                         \
    template <class T, int D>                                                \
    vec<T, D> operator op(const vec<T, D>& a, const vec<T, D>& o) {          \
        vec<T, D> out;                                                       \
        for (int i = 0; i < D; i++) out[i] = a[i] op o[i];                   \
        return out;                                                          \
    }                                                                        \
    template <class T, int D>                                                \
    vec<T, D> operator op(const vec<T, D>& a, const T & o) {                 \
        vec<T, D> out;                                                       \
        for (int i = 0; i < D; i++) out[i] = a[i] op o;                      \
        return out;                                                          \
    }                                                                        \
    template <class T, int D>                                                \
    vec<T, D> operator op(const T & a, const vec<T, D>& o) {                 \
        vec<T, D> out;                                                       \
        for (int i = 0; i < D; i++) out[i] = a op o[i];                      \
        return out;                                                          \
    }                                                                        \
    template <class T, int D>                                                \
    vec<vec<T, D>, D> operator op(const T & a, const vec<vec<T, D>, D>& o) { \
        vec<T, D> out;                                                       \
        for (int i = 0; i < D; i++) out[i] = a op o[i];                      \
        return out;                                                          \
    }                                                                        \
    template <class T, int D>                                                \
    vec<vec<T, D>, D> operator op(const vec<vec<T, D>, D>& a, const T & o) { \
        vec<T, D> out;                                                       \
        for (int i = 0; i < D; i++) out[i] = a[i] op o;                      \
        return out;                                                          \
    }

IMPL_ops(+) IMPL_ops(*) IMPL_ops(/) IMPL_ops(-);

template <class T, int D>
std::ostream& operator<<(std::ostream& os, const vec<T, D>& m) {
    os << "[";
    for (int i = 0; i < D; i++) os << (i != 0 ? ", " : "") << m[i];
    os << "]";
    return os;
}

template <class T, int D>
vec<vec<T, D>, D> transpose(const vec<vec<T, D>, D>& m) {
    vec<vec<T, D>, D> out;
    for (int i = 0; i < D; i++)
        for (int j = 0; j < D; j++) out[i][j] = m[j][i];
    return out;
}

template <class T, int D>
vec<vec<T, D>, D> mat_mul(const vec<vec<T, D>, D>& mat1,
                          const vec<vec<T, D>, D>& mat2) {
    vec<vec<T, D>, D> mat2T = transpose(mat2), out;
    for (int i = 0; i < D; i++)
        for (int j = 0; j < D; j++) out[i][j] = mat1[i].dot(mat2T[j]);
    return out;
}

template <class T, int D>
vec<T, D> mat_mul(const vec<vec<T, D>, D>& mat, const vec<T, D>& o) {
    vec<T, D> out;
    for (int i = 0; i < D; i++) out[i] = o.dot(mat[i]);
    return out;
}

template <class T, int D, int E>
vec<vec<T, D>, E> mat_mul_elementwise(const vec<vec<T, D>, D>& mat,
                                      const vec<vec<T, D>, E>& o) {
    vec<vec<T, D>, E> out;
    for (int e = 0; e < E; e++) out[e] = mat_mul(mat, o[e]);
    return out;
}

template <class T>
vec<vec<T, 3>, 3> quat_to_mat(const vec<T, 4>& q) {
    // clang-format off
    T q0=q[0],q1=q[1],q2=q[2],q3=q[3];
    vec<vec<T, 3>, 3> out {
        vec<T, 3>{2*(q0*q0+q1*q1)-1,
                  2*(q1*q2-q0*q3),
                  2*(q1*q3+q0*q2)},
        vec<T, 3>{2*(q1*q2+q0*q3),
                  2*(q0*q0+q2*q2)-1,
                  2*(q2*q3-q0*q1)},
        vec<T, 3>{2*(q1*q3-q0*q2),
                  2*(q2*q3+q0*q1),
                  2*(q0*q0+q3*q3)-1}};
    // clang-format on
    return out;
}

template <class T>
vec<vec<T, 3>, 3> cov3d(const vec<T, 3>& scale, const vec<vec<T, 3>, 3>& rot) {
    return mat_mul((transpose(rot), scale.squared().diag()), rot);
}

#define IMPL_FUN(fun)                                   \
    template <class T, int D>                           \
    vec<T, D> fun(const vec<T, D>& a) {                 \
        vec<T, D> out;                                  \
        for (int i = 0; i < D; i++) out[i] = fun(a[i]); \
        return out;                                     \
    }

using std::exp;
IMPL_FUN(exp)
using std::log;
IMPL_FUN(log)
using std::abs;
IMPL_FUN(abs)

using std::max;
using std::min;

template <class T1, class T2, int D>
vec<T1, D> min(const vec<T1, D>& a, const T2 value) {
    vec<T1, D> out;
    for (int i = 0; i < D; i++) out[i] = min(a[i], value);
    return out;
}

template <class T1, class T2, int D>
vec<T1, D> max(const vec<T1, D>& a, const T2 value) {
    vec<T1, D> out;
    for (int i = 0; i < D; i++) out[i] = max(a[i], value);
    return out;
}
};  // namespace vec
#endif