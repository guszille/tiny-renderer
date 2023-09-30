#include "model.h"

Model::Model(const char* filename)
    : vertices(), faces()
{
    std::ifstream in;
    std::string line;

    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());

        char ctrash;
        int  itrash;

        if (!line.compare(0, 2, "v "))
        {
            Vec3f vertex;

            iss >> ctrash;

            for (int i = 0; i < 3; i++) iss >> vertex.raw[i];

            vertices.push_back(vertex);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> face;
            int index;

            iss >> ctrash;

            while (iss >> index >> ctrash >> itrash >> ctrash >> itrash)
            {
                index--; // in wavefront obj all indices start at 1, not zero.

                face.push_back(index);
            }

            faces.push_back(face);
        }
    }

    std::cerr << "# v# " << vertices.size() << " f# " << faces.size() << std::endl;
}

int Model::get_nof_vertices()
{
    return (int)vertices.size();
}

int Model::get_nof_faces()
{
    return (int)faces.size();
}

Vec3f Model::get_vertex(int index)
{
    return vertices[index];
}

std::vector<int> Model::get_face(int index)
{
    return faces[index];
}
