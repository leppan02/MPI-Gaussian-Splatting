#ifndef TYPES_IMPORT
#define TYPES_IMPORT
#include "vec.h"
#include <array>
using std::array;
typedef float d_t;
typedef vec::vec<d_t, 4> v_t;
typedef vec::vec<v_t, 4> m_t;
#endif