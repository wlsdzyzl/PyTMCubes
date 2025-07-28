
#ifndef _MARCHING_CUBES_H
#define _MARCHING_CUBES_H

#include <stddef.h>
#include <array>
#include <vector>
#include <cmath>
namespace mc
{

extern int edge_table[256];
extern int triangle_table[256][16];

namespace private_
{

double mc_isovalue_interpolation(double isovalue, double f1, double f2,
    double x1, double x2);
size_t mc_add_vertex(double x1, double y1, double z1, double c2,
    int axis, double f1, double f2, double isovalue, std::vector<double>* vertices);
}

template<typename vector3, typename formula>
void marching_cubes(const vector3& lower, const vector3& upper,
    int numx, int numy, int numz, formula f, double isovalue,
    std::vector<double>& vertices, std::vector<typename vector3::size_type>& polygons, double truncated_value = 0)
{
    using coord_type = typename vector3::value_type;
    using size_type = typename vector3::size_type;
    using namespace private_;

    // Some initial checks
    if(numx < 2 || numy < 2 || numz < 2)
        return;

    if(!std::equal(std::begin(lower), std::end(lower), std::begin(upper),
                   [](double a, double b)->bool {return a <= b;}))
        return;

    // numx, numy and numz are the numbers of evaluations in each direction
    --numx; --numy; --numz;

    coord_type dx = (upper[0] - lower[0]) / static_cast<coord_type>(numx);
    coord_type dy = (upper[1] - lower[1]) / static_cast<coord_type>(numy);
    coord_type dz = (upper[2] - lower[2]) / static_cast<coord_type>(numz);

    // auto _offset = [&](size_t i, size_t j, size_t k){return i*(numy+1)*(numz+1) + j*(numz+1) + k;};

    for(int i=0; i<numx; ++i)
    {
        coord_type x = lower[0] + dx*i;
        coord_type x_dx = lower[0] + dx*(i+1);

        for(int j=0; j<numy; ++j)
        {
            coord_type y = lower[1] + dy*j;
            coord_type y_dy = lower[1] + dy*(j+1);

            double v[8];
            v[4] = f(x, y, lower[2]); v[5] = f(x_dx, y, lower[2]);
            v[6] = f(x_dx, y_dy, lower[2]); v[7] = f(x, y_dy, lower[2]);
            
            for(int k=0; k<numz; ++k)
            {
                coord_type z = lower[2] + dz*k;
                coord_type z_dz = lower[2] + dz*(k+1);

                v[0] = v[4]; v[1] = v[5];
                v[2] = v[6]; v[3] = v[7];
                v[4] = f(x, y, z_dz); v[5] = f(x_dx, y, z_dz);
                v[6] = f(x_dx, y_dy, z_dz); v[7] = f(x, y_dy, z_dz);
                bool truncated = false;
                unsigned int cubeindex = 0;
                for(int m=0; m<8; ++m)
                {
                    if(truncated_value > 0 && std::abs(v[m]) >= truncated_value)
                    {
                        truncated = true;
                        break;
                    }
                    if(v[m] <= isovalue)
                        cubeindex |= 1<<m;
                }
                if(truncated) cubeindex=0;
                // Generate vertices AVOIDING DUPLICATES.

                int edges = edge_table[cubeindex];
                std::array<size_type, 12> indices;
                if(edges & 0x040)
                {
                    size_t index = mc_add_vertex(x_dx, y_dy, z_dz, x, 0, v[6], v[7], isovalue, &vertices);
                    indices[6] = index;
                }
                if(edges & 0x020)
                {
                    size_t index = mc_add_vertex(x_dx, y, z_dz, y_dy, 1, v[5], v[6], isovalue, &vertices);
                    indices[5] = index;
                }
                if(edges & 0x400)
                {
                    size_t index = mc_add_vertex(x_dx, y_dy, z, z_dz, 2, v[2], v[6], isovalue, &vertices);
                    indices[10] = index;
                }

                if(edges & 0x001)
                {
                    size_t index = mc_add_vertex(x, y, z, x_dx, 0, v[0], v[1], isovalue, &vertices);
                    indices[0] = index;

                }
                if(edges & 0x002)
                {
                    size_t index = mc_add_vertex(x_dx, y, z, y_dy, 1, v[1], v[2], isovalue, &vertices);
                    indices[1] = index;

                }
                if(edges & 0x004)
                {
                    size_t index = mc_add_vertex(x_dx, y_dy, z, x, 0, v[2], v[3], isovalue, &vertices);
                    indices[2] = index;
                }
                if(edges & 0x008)
                {
                    size_t index = mc_add_vertex(x, y_dy, z, y, 1, v[3], v[0], isovalue, &vertices);
                    indices[3] = index;
                }
                if(edges & 0x010)
                {
                    size_t index = mc_add_vertex(x, y, z_dz, x_dx, 0, v[4], v[5], isovalue, &vertices);
                    indices[4] = index;
                }
                if(edges & 0x080)
                {
                    size_t index = mc_add_vertex(x, y_dy, z_dz, y, 1, v[7], v[4], isovalue, &vertices);
                    indices[7] = index;
                }
                if(edges & 0x100)
                {
                    size_t index = mc_add_vertex(x, y, z, z_dz, 2, v[0], v[4], isovalue, &vertices);
                    indices[8] = index;
                }
                if(edges & 0x200)
                {

                    size_t index = mc_add_vertex(x_dx, y, z, z_dz, 2, v[1], v[5], isovalue, &vertices);
                    indices[9] = index;

                }
                if(edges & 0x800)
                {
                    size_t index = mc_add_vertex(x, y_dy, z, z_dz, 2, v[3], v[7], isovalue, &vertices);
                    indices[11] = index;
                }
                
                int* triangle_table_ptr = triangle_table[cubeindex];
                for(int m=0; m < 16; m+=3)
                {   // delete degenerated faces.
                    int tri0 = triangle_table_ptr[m];
                    if(tri0 < 0) break;
                    int tri1 = triangle_table_ptr[m+1];
                    int tri2 = triangle_table_ptr[m+2];
                    if(tri0 != tri1 && tri1 != tri2 && tri2 != tri0)
                    {
                        polygons.push_back(indices[tri0]);
                        polygons.push_back(indices[tri1]);
                        polygons.push_back(indices[tri2]);
                    }
                }
            }

        }
    }

}

}

#endif // _MARCHING_CUBES_H
