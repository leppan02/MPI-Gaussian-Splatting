#include <bits/stdc++.h>

#include "cpp_impl/camera.h"
#include "cpp_impl/default_types.h"
#include "cpp_impl/vec.h"

using namespace std;


int main() {
    vec::vec<v_t, 100> data;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            data[i * 10 + j] = v_t{(d_t)i, (d_t)j, 0, 1};
        }
    }
    data = data - v_t{4.5, 4.5};
    for(int it = 0; it < 10; it++){
        auto cam = camera::Camera();
        cam.image_size_x = cam.image_size_y = 32;
        cam.roll(it * M_PI/10.);
        cam.move(v_t{0,0,10});
        auto trans = vec::mat_mul_elementwise(cam.get_matrix(), data);
        for (auto &v : trans) {
            float depth = v[2];
            v = v / depth;
            v[2] = depth;
        }
        char canvas[cam.image_size_x][cam.image_size_y];
        for (int i = 0; i < cam.image_size_x; i++)
            for (int j = 0; j < cam.image_size_y; j++) canvas[i][j] = ' ';
        for (auto v : trans) {
            int a = v[0];
            int b = v[1];
            if (a > -1 && a < cam.image_size_x && b > -1 && b < cam.image_size_y) canvas[a][b] = '#';
        }
        for (int j = 0; j < cam.image_size_y; j++) cout << "#";
        cout<<endl;
        for (int i = 0; i < cam.image_size_x; i++) {
            for (int j = 0; j < cam.image_size_y; j++) cout << canvas[i][j];
            cout << endl;
        }
        for (int j = 0; j < cam.image_size_y; j++) cout << "#";
        cout << endl;
    }
}