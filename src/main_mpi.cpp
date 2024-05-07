#include "generate_image.hpp"

int main() {
    Camera cam(1000, 1000, (d_t)M_PI / 2.f);

    std::string f_name = "data/point_cloud.ply";
    happly::PLYData ply_data(f_name);
    GaussianData data;

    std::vector<int> el(GaussianData::get_size(ply_data));
    std::iota(el.begin(), el.end(), 0);

    auto xyz = GaussianData::load_xyz(ply_data, el);
    const int TREE_DEPTH = 1;
    vector<Image> images;
    QuadTree qt(TREE_DEPTH, xyz);
    v4_t mn = qt.children[0].mn, mx = qt.children[0].mx;
    for (auto const &c: qt.children){
        mn = min(mn, c.mn);
        mx = max(mx, c.mx);
    }
    cam.move_to((mn + mx) / 2.f);  
    cam.tilt(-(d_t)M_PI / 4.f);
    cam.tilt((d_t)M_PI / 8.f);
    cam.move_to(v4_t{0, 0, -1.5});
    
    auto order = qt.order(cam.global_position());
    DEBUG_PRINT("Order ")
    for(auto const &o: order){
        DEBUG_PRINT(o)
    }
    DEBUG_PRINT("Step 1")
    DEBUG_PRINT(qt.idx.size())
    for(auto const &c: qt.children){
        DEBUG_PRINT(c.idx.size())
        data.load_data(ply_data, c.idx);
        DEBUG_PRINT("Data loaded")
        images.emplace_back(render(cam, data));
        DEBUG_PRINT("Image rendered")
    }
    for (size_t i = 1; i < order.size(); i++){
        images[order[0]].combine(images[order[i]]);
    }
    images[order[0]].store_image("img.bmp");
    return 0;
}