#include<vector>
using vector = std::vector;
struct QuadTree {
    vector<int> idx;
    vector<QuadNode> children;
    vector<vector<v4_t>> cuts;
    const v4_t dirs[3] = {v4_t{1, 0, 0, 0}, v4_t{0, 1, 0, 0}, v4_t{0, 0, 1, 0}}; 
    int depth;
    QuadTree(int _depth, const vector<v4_t> &xyz): depth(_depth) {
        idx = vector<int>(xyz.size());
        std::iota(idx.begin(), idx.end(), 0);
        vector<vector<std::span<int>>> views;
        views.push_back({std::span<int>(idx)});
        for (int i = 0; i < depth; i++) {
            views.push_back({});
            cuts.push_back({});
            auto dir = dirs[i % 3];
            for (auto v : views[i]) {
                sort_span_in_direction(xyz, dir, v);
                auto s1 = v.subspan(0, v.size() / 2);
                auto s2 = v.subspan(v.size() / 2);
                views[i + 1].push_back(s1);
                views[i + 1].push_back(s2);
                cuts[i].push_back({xyz[s1.back()]});
            }
        }
        for (auto v : views[depth]) {
            children.emplace_back(v, xyz);
        }
    }
    vector<int> order(v4_t camera_pos){
        vector<vector<std::tuple<int, int>>> sorted_ranges({{std::tuple<int, int>(0, children.size())}});
        for (int i = 0; i < depth; i++) {
            auto dir = dirs[i % 3];
            vector<std::tuple<int, int>> new_range = {};
            for(size_t j = 0; j < cuts[i].size(); j++){
                auto [start, end] = sorted_ranges.back()[j];
                int mid = (start + end) / 2;
                std::tuple<int, int>first_half = {start, mid}, second_half = {mid, end};

                if (camera_pos.dot(dir) > cuts[i][j].dot(dir)){
                    new_range.push_back(first_half);
                    new_range.push_back(second_half);
                }else{
                    new_range.push_back(second_half);
                    new_range.push_back(first_half);
                }
            }
            sorted_ranges.push_back(new_range);
        }
        vector<int> result;
        for(auto [start, _end] : sorted_ranges.back()){
            result.push_back(start);
        }
        return result;
    }
};
