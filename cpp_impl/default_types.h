#ifndef TYPES_IMPORT
#define TYPES_IMPORT
#include <array>

#include "vec.h"
using std::array;
typedef float d_t;
typedef vec::vec<d_t, 3> v3_t;
typedef vec::vec<v3_t, 3> m3_t;
typedef vec::vec<d_t, 4> v4_t;
typedef vec::vec<v4_t, 4> m4_t;
#endif