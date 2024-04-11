#include <algorithm>
#include <numeric>
#include <vector>

#include "color.h"
#include "default_types.h"
#include "vec.h"

using std::iota;
using std::reverse;
using std::vector;

vector<int> sort_positions_in_direction(const vector<v4_t> &pos,
                                        const v3_t &dir_) {
    vector<d_t> depth;
    v4_t dir{dir_[0], dir_[1], dir_[2]};
    for (const auto &v : pos) depth.emplace_back(v.dot(dir));

    vector<int> idx(pos.size());
    iota(idx.begin(), idx.end(), 0);
    stable_sort(idx.begin(), idx.end(), [&depth](int i1, int i2) {
        return depth[i1] < depth[i2];
    });

    return idx;
}

struct Scene {
    vector<ColorHarmonic> colors;
    vector<v4_t> pos;
    vector<m3_t> cov3d;
    vector<vector<int>> index_sorting;
    vector<v3_t> sorting_direction;
    void pre_compute_sorting(vector<v3_t> dirs) {
        for (auto dir : dirs) {
            auto idx = sort_positions_in_direction(pos, dir);
            index_sorting.push_back(idx);
            sorting_direction.push_back(dir);

            // Store reversed index
            reverse(idx.begin(), idx.end());
            index_sorting.push_back(idx);
            sorting_direction.push_back((d_t)-1. * dir);
        }
    }
};