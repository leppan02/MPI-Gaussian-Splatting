#include <math.h>

#include <array>
#include <iostream>
#include <ostream>

using namespace std;

template <class T, int D>
struct vec : public array<T, D> {
    typedef vec<T, D> _v;
    typedef vec<_v, D> _m;
    inline T sum() const {
        T out = 0;
        for (int i = 0; i < D; i++) out += (*this)[i];
        return out;
    }
    inline T dot(const _v& o) const { return ((*this) * o).sum(); }
    inline _v squared() const { return (*this) * (*this); }
    inline T norm() const { return (*this).squared().sum(); }

    inline _m diag() const {
        _m out{};
        for (int i = 0; i < D; i++) out[i][i] = (*this)[i];
        return out;
    }
};

#define IMPL_ops(op)                                                       \
    template <class T, int D>                                              \
    inline vec<T, D> operator op(const vec<T, D>& a, const vec<T, D>& o) { \
        vec<T, D> out;                                                     \
        for (int i = 0; i < D; i++) out[i] = a[i] op o[i];                 \
        return out;                                                        \
    }                                                                      \
    template <class T, int D>                                              \
    inline vec<T, D> operator op(const vec<T, D>& a, const T & o) {        \
        vec<T, D> out;                                                     \
        for (int i = 0; i < D; i++) out[i] = a[i] op o;                    \
        return out;                                                        \
    }                                                                      \
    template <class T, int D>                                              \
    inline vec<T, D> operator op(const T & a, const vec<T, D>& o) {        \
        vec<T, D> out;                                                     \
        for (int i = 0; i < D; i++) out[i] = a op o[i];                    \
        return out;                                                        \
    }                                                                      \
    template <class T, int D>                                              \
    inline vec<vec<T, D>, D> operator op(const T & a,                      \
                                         const vec<vec<T, D>, D>& o) {     \
        vec<T, D> out;                                                     \
        for (int i = 0; i < D; i++) out[i] = a op o[i];                    \
        return out;                                                        \
    }                                                                      \
    template <class T, int D>                                              \
    inline vec<vec<T, D>, D> operator op(const vec<vec<T, D>, D>& a,       \
                                         const T & o) {                    \
        vec<T, D> out;                                                     \
        for (int i = 0; i < D; i++) out[i] = a[i] op o;                    \
        return out;                                                        \
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
inline vec<vec<T, D>, D> transpose(const vec<vec<T, D>, D>& m) {
    vec<vec<T, D>, D> out;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) out[i][j] = m[j][i];
    return out;
}

template <class T, int D>
inline vec<vec<T, D>, D> mat_mul(const vec<vec<T, D>, D>& mat1,
                                 const vec<vec<T, D>, D>& mat2) {
    vec<vec<T, D>, D> mat2T = transpose(mat2), out;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[i][j] = out[i][j] + mat1[i].dot(mat2T[j]);
    return out;
}

template <class T, int D>
inline vec<T, D> mat_mul(const vec<vec<T, D>, D>& mat, const vec<T, D>& o) {
    return vec<T, D>{o.dot(mat[0]), o.dot(mat[1]), o.dot(mat[2]),
                     o.dot(mat[3])};
}

#define IMPL_FUN(fun)                                                 \
    template <class T, int D>                                         \
    inline vec<T, D> fun(const vec<T, D>& a) {                        \
        return vec<T, D>{fun(a[0]), fun(a[1]), fun(a[2]), fun(a[3])}; \
    }

IMPL_FUN(exp)
IMPL_FUN(log)
IMPL_FUN(abs)