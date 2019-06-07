#include "stlLoader.h"


int read_binary_STL_file(std::string STL_filename,std::vector<triangle> & facet,
                         float & x_min, float & x_max, float & y_min, float & y_max, float & z_min, float & z_max)
{
    // declare an (input) file object
    std::ifstream binaryInputFile;

    // open the STL file by using the full path and the name
    // specify that the file is opened in "read-only" mode
    // specify that the file is opened in binary format
    binaryInputFile.open((STL_filename).c_str(), std::ifstream::in | std::ifstream::binary);

    // check whether the file was opened successfully
    // if yes then continue otherwise terminate program execution
    if(binaryInputFile.fail())
    {
        std::cout << "ERROR: Input STL file could not be opened!" << std::endl;
        return (1);
    }

    // position the pointer to byte number 80
    binaryInputFile.seekg(80);

    // read the number of facets (triangles) in the STL geometry
    int numberOfTriangles;
    binaryInputFile.read((char*) &numberOfTriangles, sizeof(int));

    std::cout << "There are " << numberOfTriangles << " triangles." << std::endl; 
    // declare an object "tri" of type triangle (see main.h for the definition of the triangle class)
    triangle tri;

    // storage space for the "unused bytes" 
    char unused_bytes[2];

    // initialize parameters that will be used to store the minimum and maximum extents of the geometry
    // described by the STL file
    x_min = 1e+30, x_max = -1e+30;
    y_min = 1e+30, y_max = -1e+30;
    z_min = 1e+30, z_max = -1e+30;

    // temporary floating point variable
    float temp_float_var;

    for(int count=0;count<numberOfTriangles;count++)
    {
        // read the three components of the normal vector
        binaryInputFile.read((char*)&temp_float_var,4); tri.normal.x = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.normal.y = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.normal.z = (float) temp_float_var;

        // read the three coordinates of vertex 1 
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[0].x = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[0].y = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[0].z = (float) temp_float_var;

        // read the three coordinates of vertex 2 
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[1].x = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[1].y = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[1].z = (float) temp_float_var;

        // read the three coordinates of vertex 3
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[2].x = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[2].y = (float) temp_float_var;
        binaryInputFile.read((char*)&temp_float_var,4); tri.point[2].z = (float) temp_float_var;

        // read the 2 unused bytes
        binaryInputFile.read(unused_bytes,2);

        // update geometry extents along X, Y and Z based on vertex 1 of this triangle
        if (tri.point[0].x < x_min) x_min = tri.point[0].x;
        if (tri.point[0].x > x_max) x_max = tri.point[0].x;
        if (tri.point[0].y < y_min) y_min = tri.point[0].y;
        if (tri.point[0].y > y_max) y_max = tri.point[0].y;
        if (tri.point[0].z < z_min) z_min = tri.point[0].z;
        if (tri.point[0].z > z_max) z_max = tri.point[0].z;

        // update geometry extents along X, Y and Z based on vertex 2 of this triangle
        if (tri.point[1].x < x_min) x_min = tri.point[1].x;
        if (tri.point[1].x > x_max) x_max = tri.point[1].x;
        if (tri.point[1].y < y_min) y_min = tri.point[1].y;
        if (tri.point[1].y > y_max) y_max = tri.point[1].y;
        if (tri.point[1].z < z_min) z_min = tri.point[1].z;
        if (tri.point[1].z > z_max) z_max = tri.point[1].z;

        // update geometry extents along X, Y and Z based on vertex 3 of this triangle
        if (tri.point[2].x < x_min) x_min = tri.point[2].x;
        if (tri.point[2].x > x_max) x_max = tri.point[2].x;
        if (tri.point[2].y < y_min) y_min = tri.point[2].y;
        if (tri.point[2].y > y_max) y_max = tri.point[2].y;
        if (tri.point[2].z < z_min) z_min = tri.point[2].z;
        if (tri.point[2].z > z_max) z_max = tri.point[2].z;

        // add data for this triangle to the "facet" vector
        facet.push_back(tri);
    }

    // explicitly close the connection to the input STL file
    binaryInputFile.close();

    return (0);  // all is well
}




STL_shape::STL_shape(std::string filename)
{
	std::cout << "Reading binary file" << std::endl;
	read_binary_STL_file(filename, m_triangles, x_min, x_max, y_min, y_max, z_min, z_max);
	midx = (x_min + x_max) / 2.0;
	midy = (y_min + y_max) / 2.0;
	midz = (z_min + z_max) / 2.0;

	//FILL VERTICES ARRAY
	std::cout << "Filling in Vertices Array" << std::endl;

	num_vertices = m_triangles.size() * 3;
	vertices = (float*) malloc(sizeof(float) * num_vertices * 6);

	std::cout << "There are " << num_vertices << " vertices" << std::endl;

	int count = 0;
	for(int i = 0; i < m_triangles.size(); i++)
	{
		vertices[ count ] = m_triangles[i].point[0].x - midx; 
		vertices[count+1] = m_triangles[i].point[0].y - midy;
		vertices[count+2] = m_triangles[i].point[0].z - midz;
		vertices[count+3] = m_triangles[i].normal.x;
		vertices[count+4] = m_triangles[i].normal.y;
		vertices[count+5] = m_triangles[i].normal.z;

		vertices[count+6] = m_triangles[i].point[1].x - midx; 
		vertices[count+7] = m_triangles[i].point[1].y - midy;
		vertices[count+8] = m_triangles[i].point[1].z - midz;
		vertices[count+9] = m_triangles[i].normal.x;
		vertices[count+10] = m_triangles[i].normal.y;
		vertices[count+11] = m_triangles[i].normal.z;

		vertices[count+12] = m_triangles[i].point[2].x - midx; 
		vertices[count+13] = m_triangles[i].point[2].y - midy;
		vertices[count+14] = m_triangles[i].point[2].z - midz;
		vertices[count+15] = m_triangles[i].normal.x;
		vertices[count+16] = m_triangles[i].normal.y;
		vertices[count+17] = m_triangles[i].normal.z;

		count += 18;
	}

    midx = midy = midz = 0.0;


}

STL_shape::~STL_shape()
{
	free(vertices);
}
