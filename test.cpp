#include <bits/stdc++.h>
using namespace std;

template <typename el_T, size_t C>
struct vec : public array<el_T, C> {
    auto sum() const {
        el_T ret = 0;
        for (int i = 0; i < C; i++) ret += (*this)[i];
        return ret;
    }
    template <size_t CB>
    auto dot(const vec<el_T, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        return ((*this) * B).sum();
    }
    auto squared() const {
        vec<el_T, C> ret;
        for (int i = 0; i < C; i++) ret[i] = (*this)[i] * (*this)[i];
        return ret;
    }
    auto norm2() const { return (*this).squared().sum(); }
    auto normalized() const { return (*this) / sqrt((*this).squared().sum()); }
};

template <typename el_T, size_t R, size_t C>
struct mat : public array<vec<el_T, C>, R> {
    auto T() const {
        mat<el_T, C, R> O;
        for (int i = 0; i < C; i++)
            for (int j = 0; j < R; j++) O[i][j] = (*this)[j][i];
        return O;
    }
    template <size_t RB, size_t CB>
    auto mat_mul_T(const mat<el_T, RB, CB>& B) const {
        static_assert(C == CB, "Wrong dimensions");
        mat<el_T, R, RB> O;
        for (int i = 0; i < R; i++)
            for (int j = 0; j < RB; j++) O[i][j] = (*this)[i].dot(B[j]);
        return O;
    }
    template <size_t RB, size_t CB>
    auto mat_mul(const mat<el_T, RB, CB>& B) const {
        static_assert(C == RB, "Wrong dimensions");
        return mat_mul_T(B.T());
    }
};

template <typename el_T, size_t CA>
auto diag(vec<el_T, CA> A) {
    mat<el_T, CA, CA> O = {0};
    for (int i = 0; i < CA; i++) O[i] = A[i];
    return O;
}

template <typename T>
auto clip_0_1(T a) {
    return max(min(a, (T)1), (T)0);
}

#define IMPL_fun(fun)                                        \
    template <typename el_T, size_t CA>                      \
    auto fun(const vec<el_T, CA>& A) {                       \
        vec<el_T, CA> O;                                     \
        for (int i = 0; i < CA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename el_T, size_t RA, size_t CA>           \
    auto fun(const mat<el_T, RA, CA>& A) {                   \
        mat<el_T, RA, CA> O;                                 \
        for (int i = 0; i < CA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename TA, size_t NA>                        \
    auto fun(const array<TA, NA>& A) {                       \
        array<TA, NA> O;                                     \
        for (int i = 0; i < NA; i++) O[i] = fun(A[i]);       \
        return O;                                            \
    }                                                        \
    template <typename TA>                                   \
    auto fun(const vector<TA>& A) {                          \
        vector<TA> O(A.size());                              \
        for (int i = 0; i < A.size(); i++) O[i] = fun(A[i]); \
        return O;                                            \
    }

IMPL_fun(sqrt) IMPL_fun(exp) IMPL_fun(log) IMPL_fun(clip_0_1) IMPL_fun(abs);

#define IMPL_ops(op)                                                           \
    template <typename el_T, size_t CA, size_t CB>                             \
    auto operator op(const vec<el_T, CA>& A, const vec<el_T, CB>& B) {         \
        static_assert(CA == CB, "Wrong dimensions");                           \
        vec<el_T, CA> O;                                                       \
        for (int i = 0; i < CA; i++) O[i] = A[i] op B[i];                      \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA, size_t RB, size_t CB>       \
    auto operator op(const mat<el_T, RA, CA>& A, const mat<el_T, RB, CB>& B) { \
        static_assert(RA == RB && CA == CB, "Wrong dimensions");               \
        mat<el_T, RA, CA> O;                                                   \
        for (int i = 0; i < CA; i++) O[i] = A[i] op B[i];                      \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t CA>                                        \
    auto operator op(const vec<el_T, CA>& A, el_T b) {                         \
        vec<el_T, CA> O;                                                       \
        for (int i = 0; i < CA; i++) O[i] = A[i] op b;                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t CA>                                        \
    auto operator op(el_T a, const vec<el_T, CA>& B) {                         \
        vec<el_T, CA> O;                                                       \
        for (int i = 0; i < CA; i++) O[i] = a op B[i];                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA>                             \
    auto operator op(const mat<el_T, RA, CA>& A, el_T b) {                     \
        mat<el_T, RA, CA> O;                                                   \
        for (int i = 0; i < CA; i++) O[i] = A[i] op b;                         \
        return O;                                                              \
    }                                                                          \
    template <typename el_T, size_t RA, size_t CA>                             \
    auto operator op(el_T a, const mat<el_T, RA, CA>& B) {                     \
        mat<el_T, RA, CA> O;                                                   \
        for (int i = 0; i < CA; i++) O[i] = a op B[i];                         \
        return O;                                                              \
    }

IMPL_ops(+) IMPL_ops(*) IMPL_ops(/) IMPL_ops(-);

template <typename el_T, size_t C>
std::ostream& operator<<(std::ostream& os, const vec<el_T, C>& m) {
    os << "[";
    for (int i = 0; i < m.size(); i++) os << (i != 0 ? ", " : "") << m[i];
    os << "]";
    return os;
}

template <typename el_T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const mat<el_T, R, C>& m) {
    os << "[";
    for (int i = 0; i < m.size(); i++) os << (i != 0 ? ",\n " : "") << m[i];
    os << "]";
    return os;
}

template <typename el_T, size_t N, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const array<mat<el_T, R, C>, N>& m) {
    os << "{";
    for (int i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename el_T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const vector<mat<el_T, R, C>>& m) {
    os << "{";
    for (int i = 0; i < m.size(); i++)
        os << (i != 0 ? ",\n " : "") << i << ":\n" << m[i];
    os << "}";
    return os;
}

template <typename TA, typename TB, typename TO, size_t NA, size_t NB>
auto broadcast(array<TA, NA> A, array<TB, NB> B, TO (*func)(TA, TB)) {
    static_assert(NA == 1 || NB == 1 || NA == NB,
                  "Can not determine broadcast rules");
    constexpr size_t N = max(NA, NB);
    array<TO, N> out;
    constexpr bool inc_a = NA != 1;
    constexpr bool inc_b = NB != 1;
    for (int i = 0; i < N; i++) {
        int ia = inc_a ? i : 0, ib = inc_b ? i : 0;
        out[i] = func(A[ia], (B[ib]));
    }
    return out;
}

template <typename TA, typename TO, size_t NA>
auto apply(array<TA, NA> A, TO (*func)(TA)) {
    array<TO, NA> out;
    for (int i = 0; i < NA; i++) out[i] = func(A[i]);
    return out;
}

template <typename TA, typename TB, typename TO>
auto broadcast(vector<TA> A, vector<TB> B, TO (*func)(TA, TB)) {
    size_t NA = A.size(), NB = B.size();
    assert(NA == 1 || NB == 1 ||
           NA == NB);  // Can not determine broadcast rules
    size_t N = max(NA, NB);
    vector<TO> out(N);
    bool inc_a = NA != 1;
    bool inc_b = NB != 1;
    for (int i = 0; i < N; i++) {
        int ia = inc_a ? i : 0, ib = inc_b ? i : 0;
        out[i] = func(A[ia], (B[ib]));
    }
    return out;
}

template <typename TA, typename TO>
auto apply(vector<TA> A, TO (*func)(TA)) {
    vector<TO> out(A.size());
    for (int i = 0; i < A.size(); i++) out[i] = func(A[i]);
    return out;
}

int main() {
    vector<mat<float, 3, 3>> test(3);
    test = apply(test, +[](mat<float, 3, 3> a) { return a + 3.f; });
    test = broadcast(
        test, test,
        +[](mat<float, 3, 3> a, mat<float, 3, 3> b) { return a * b; });
    std::cout << sqrt(test) << std::endl;
}