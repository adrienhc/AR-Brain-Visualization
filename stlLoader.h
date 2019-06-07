#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct vec3d
{
    float x; 
    float y;  
    float z;  
};

class triangle
{
public:
    vec3d normal;
    vec3d point[3];
};

class STL_shape
{
    public:
        STL_shape(std::string filename);
        ~STL_shape();

        std::vector <triangle> m_triangles;
        float x_min, x_max, y_min, y_max, z_min, z_max;

        float midx;
        float midy;
        float midz;

        int num_vertices;
        float* vertices;

};
