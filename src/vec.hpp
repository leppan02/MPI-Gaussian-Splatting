#ifndef VEC_IMPORT
#define VEC_IMPORT 1

#include <math.h>

#include <array>
#include <cassert>
#include <ostream>
#include <vector>

using std::array;
using std::max;
using std::min;
using std::vector;
/**
 * @brief A generic vector class with compile-time size.
 * 
 * This class represents a vector with a fixed size, specified at compile-time.
 * It inherits from the std::array class and provides additional functionality for vector operations.
 * 
 * @tparam el_T The element type of the vector.
 * @tparam C The size of the vector.
 */
template <typename el_T, size_t C>
struct vec : public array<el_T, C> {
    /**
     * @brief Calculates the sum of all elements in the vector.
     * 
     * @return The sum of all elements in the vector.
     */
    auto sum() const {
        el_T ret = 0;
        for (size_t i = 0; i < C; i++) ret += (*this)[i];
        return ret;
    }

    /**
     * @brief Calculates the dot product between this vector and another vector.
     * 
     * @tparam CB The size of the other vector.
     * @param B The other vector.
     * @return The dot product between this vector and the other vector.
     */
    template <size_t CB>
    auto dot(const vec<el_T, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        return ((*this) * B).sum();
    }

    /**
     * @brief Calculates the element-wise square of the vector.
     * 
     * @return A new vector with each element squared.
     */
    auto squared() const {
        vec<el_T, C> ret;
        for (size_t i = 0; i < C; i++) ret[i] = (*this)[i] * (*this)[i];
        return ret;
    }

    /**
     * @brief Calculates the squared Euclidean norm of the vector.
     * 
     * @return The squared Euclidean norm of the vector.
     */
    auto norm2() const { return (*this).squared().sum(); }

    /**
     * @brief Calculates the normalized version of the vector.
     * 
     * @return A new vector with the same direction as the original vector, but with unit length.
     */
    auto normalized() const { return (*this) / sqrt((*this).squared().sum()); }

    /**
     * @brief Resizes the vector to a new size.
     * 
     * If the new size is larger than the current size, the additional elements are initialized with the default value of the element type.
     * If the new size is smaller than the current size, the extra elements are discarded.
     * 
     * @tparam CO The new size of the vector.
     * @return A new vector with the resized elements.
     */
    template <size_t CO>
    auto resized() const {
        if (CO >= C) {
            return vec<el_T, CO>{(*this)};
        }
        vec<el_T, CO> O;
        for (size_t i = 0; i < CO; i++) O[i] = (*this)[i];
        return O;
    }
};

/**
 * @brief A matrix class that extends the functionality of the array class.
 * 
 * @tparam el_T The element type of the matrix.
 * @tparam R The number of rows in the matrix.
 * @tparam C The number of columns in the matrix.
 */
template <typename el_T, size_t R, size_t C>
struct mat : public array<vec<el_T, C>, R> {
    /**
     * @brief Transposes the matrix.
     * 
     * @return The transposed matrix.
     */
    auto T() const {
        mat<el_T, C, R> O;
        for (size_t i = 0; i < C; i++)
            for (size_t j = 0; j < R; j++) O[i][j] = (*this)[j][i];
        return O;
    }

    /**
     * @brief Performs matrix multiplication with the transpose of another matrix.
     * 
     * @tparam RB The number of rows in the other matrix.
     * @tparam CB The number of columns in the other matrix.
     * @param B The other matrix.
     * @return The result of the matrix multiplication.
     */
    template <size_t RB, size_t CB>
    auto mat_mul_T(const mat<el_T, RB, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        mat<el_T, R, RB> O;
        for (size_t i = 0; i < R; i++)
            for (size_t j = 0; j < RB; j++) O[i][j] = (*this)[i].dot(B[j]);
        return O;
    }

    /**
     * @brief Performs matrix multiplication with a vector.
     * 
     * @tparam CB The number of elements in the vector.
     * @param B The vector.
     * @return The result of the matrix multiplication.
     */
    template <size_t CB>
    auto mat_mul(const vec<el_T, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        vec<el_T, R> O;
        for (size_t i = 0; i < R; i++) O[i] = (*this)[i].dot(B);
        return O;
    }

    /**
     * @brief Performs matrix multiplication with a diagonal matrix.
     * 
     * @tparam CB The number of elements in the diagonal matrix.
     * @param B The diagonal matrix.
     * @return The result of the matrix multiplication.
     */
    template <size_t CB>
    auto mat_mul_diag(const vec<el_T, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        mat<el_T, R, CB> O;
        for (size_t i = 0; i < R; i++)
            for (size_t j = 0; j < C; j++) O[i][j] = (*this)[i][j] * B[j];
        return O;
    }

    /**
     * @brief Performs matrix multiplication with another matrix.
     * 
     * @tparam RB The number of rows in the other matrix.
     * @tparam CB The number of columns in the other matrix.
     * @param B The other matrix.
     * @return The result of the matrix multiplication.
     */
    template <size_t RB, size_t CB>
    auto mat_mul(const mat<el_T, RB, CB>& B) const {
        static_assert(C == RB, "Wrong dimensions");
        return mat_mul_T(B.T());
    }

    /**
     * @brief Resizes the matrix to a new size.
     * 
     * @tparam RO The number of rows in the resized matrix.
     * @tparam CO The number of columns in the resized matrix.
     * @return The resized matrix.
     */
    template <size_t RO, size_t CO>
    auto resized() const {
        mat<el_T, RO, CO> O{0};
        for (size_t i = 0; i < RO && i < R; i++) O[i] = resize((*this)[i]);
        return O;
    }
};

template <typename el_T, size_t CA>
/**
 * Computes the diagonal matrix from a given vector.
 *
 * @param A The input vector.
 * @return The diagonal matrix.
 */
auto diag(vec<el_T, CA> A) {
    mat<el_T, CA, CA> O = {0};
    for (size_t i = 0; i < CA; i++) O[i][i] = A[i];
    return O;
}

#define IMPL_fun(fun)                                        \
    template <typename el_T, size_t CA>                      \
    auto fun(const vec<el_T, CA>& A) {                       \
        vec<el_T, CA> O;                                     \
        for (size_t i = 0; i < CA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename el_T, size_t RA, size_t CA>           \
    auto fun(const mat<el_T, RA, CA>& A) {                   \
        mat<el_T, RA, CA> O;                                 \
        for (size_t i = 0; i < CA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename TA, size_t NA>                        \
    auto fun(const array<TA, NA>& A) {                       \
        array<TA, NA> O;                                     \
        for (size_t i = 0; i < NA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename TA>                                   \
    auto fun(const vector<TA>& A) {                          \
        vector<TA> O(A.size());                              \
        for (size_t i = 0; i < A.size(); i++) O[i] = fun(A[i]); \
        return O;                                            \
    }

using std::abs;
using std::exp;
using std::log;
using std::sqrt;
IMPL_fun(sqrt) IMPL_fun(exp) IMPL_fun(log) IMPL_fun(abs);

#define IMPL_ops(op)                                                           \
    template <typename el_T, size_t CA, size_t CB>                             \
    auto operator op(const vec<el_T, CA>& A, const vec<el_T, CB>& B) {         \
        static_assert(CA == CB, "Wrong dimensions");                           \
        vec<el_T, CA> O;                                                       \
        for (size_t i = 0; i < CA; i++) O[i] = A[i] op B[i];                      \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA, size_t RB, size_t CB>       \
    auto operator op(const mat<el_T, RA, CA>& A, const mat<el_T, RB, CB>& B) { \
        static_assert(RA == RB && CA == CB, "Wrong dimensions");               \
        mat<el_T, RA, CA> O;                                                   \
        for (size_t i = 0; i < CA; i++) O[i] = A[i] op B[i];                      \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t CA>                                        \
    auto operator op(const vec<el_T, CA>& A, el_T b) {                         \
        vec<el_T, CA> O;                                                       \
        for (size_t i = 0; i < CA; i++) O[i] = A[i] op b;                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t CA>                                        \
    auto operator op(el_T a, const vec<el_T, CA>& B) {                         \
        vec<el_T, CA> O;                                                       \
        for (size_t i = 0; i < CA; i++) O[i] = a op B[i];                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA>                             \
    auto operator op(const mat<el_T, RA, CA>& A, el_T b) {                     \
        mat<el_T, RA, CA> O;                                                   \
        for (size_t i = 0; i < CA; i++) O[i] = A[i] op b;                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA>                             \
    auto operator op(el_T a, const mat<el_T, RA, CA>& B) {                     \
        mat<el_T, RA, CA> O;                                                   \
        for (size_t i = 0; i < CA; i++) O[i] = a op B[i];                         \
        return O;                                                              \
    }

IMPL_ops(+) IMPL_ops(*) IMPL_ops(/) IMPL_ops(-);

template <typename el_T, size_t C>
std::ostream& operator<<(std::ostream& os, const vec<el_T, C>& m) {
    os << "[";
    for (size_t i = 0; i < m.size(); i++) os << (i != 0 ? ", " : "") << m[i];
    os << "]";
    return os;
}

template <typename el_T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const mat<el_T, R, C>& m) {
    os << "[";
    for (size_t i = 0; i < m.size(); i++) os << (i != 0 ? ",\n " : "") << m[i];
    os << "]";
    return os;
}

template <typename el_T, size_t N, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const array<mat<el_T, R, C>, N>& m) {
    os << "{";
    for (size_t i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename el_T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const vector<mat<el_T, R, C>>& m) {
    os << "{";
    for (size_t i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename el_T, size_t N, size_t C>
std::ostream& operator<<(std::ostream& os, const array<vec<el_T, C>, N>& m) {
    os << "{";
    for (size_t i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename el_T, size_t C>
std::ostream& operator<<(std::ostream& os, const vector<vec<el_T, C>>& m) {
    os << "{";
    for (size_t i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename TA, typename TB, typename TO, typename _Function>
auto broadcast_to(TA A, TB B, vector<TO> O, _Function __f) {
    size_t NA = A.size(), NB = B.size();
    assert(NA == 1 || NB == 1 ||
           NA == NB);  // Can not determine broadcast rules
    size_t N = max(NA, NB);
    O.clear();
    bool inc_a = NA != 1;
    bool inc_b = NB != 1;
    for (size_t i = 0; i < N; i++) {
        int ia = inc_a ? i : 0, ib = inc_b ? i : 0;
        O.emplace_back(__f(A[ia], (B[ib])));
    }
}

template <typename TA, typename TB, typename TO, size_t NO, typename _Function>
auto broadcast_to(TA A, TB B, array<TO, NO> O, _Function __f) {
    size_t NA = A.size(), NB = B.size();
    assert(NA == 1 || NB == 1 ||
           NA == NB);  // Can not determine broadcast rules
    size_t N = max(NA, NB);
    assert(NO == N);
    bool inc_a = NA != 1;
    bool inc_b = NB != 1;
    for (size_t i = 0; i < N; i++) {
        int ia = inc_a ? i : 0, ib = inc_b ? i : 0;
        O[i] = __f(A[ia], (B[ib]));
    }
}

#endif