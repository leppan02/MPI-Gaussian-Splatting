#include <bits/stdc++.h>
#include "arr.h"

using namespace std;

float SH_C[16] = {0.28209479177387814, 0.4886025119029199, 1.0925484305920792, -1.0925484305920792, 0.31539156525252005, -1.0925484305920792, 0.5462742152960396, -0.5900435899266435, 2.890611442640554, -0.4570457994644658, 0.3731763325901154, -0.4570457994644658, 1.445305721320277, -0.5900435899266435};

const float G_WIDTH = 1920, G_HEIGHT = 1080;
const float SCALE_MODIFIER = 1.0;

typedef valarray<float> Color;
typedef valarray<float>  Vec3;

class Gaussian
{
    vector<Color> sh;

public:
    Color get_color(Vec3 dir, int shdim)
    {
        Color color = sh[0];
        
        Vec3 dir2[3];

        switch (shdim)
        {
        case 48:
            dir2[0] = dir * dir[0];
            dir2[1] = dir * dir[1];
            dir2[2] = dir * dir[2];

            color += sh[9] * dir[1] * (3.0 * dir2[0][0] - dir2[1][1]);
            color += sh[10] * dir2[0][1] * dir[2];
            color += sh[11] * dir[1] * (4.0 * dir2[2][2] - dir2[0][0] - dir2[1][1]);
            color += sh[12] * dir[2] * (2.0 * dir2[2][2] - 3.0 * dir2[0][0] - 3.0 * dir2[1][1]);
            color += sh[13] * dir[0] * (4.0 * dir2[2][2] - dir2[0][0] - dir2[1][1]);
            color += sh[14] * dir[2] * (dir2[0][0] - dir2[1][1]);
            color += sh[15] * dir[0] * (dir2[0][0] - 3.0 * dir2[1][1]);
        case 27:
            color += sh[4] * dir2[0][1];
            color += sh[5] * dir2[1][2];
            color += sh[6] * (2.0 * dir2[2][2] - dir2[0][0] - dir2[1][1]);
            color += sh[7] * dir2[0][2];
            color += sh[8] * (dir2[0][0] - dir2[1][1]);
        case 12:
            color -= sh[1] * dir[1];
            color -= sh[2] * dir[2];
            color -= sh[3] * dir[0];
        }
        color += 0.5;
        color = color.apply([](float a){return min(max((float)0., a), (float)1.);});
        return color;
    }
};

int main()
{
}