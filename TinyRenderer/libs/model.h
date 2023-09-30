#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "geometry.h"

class Model
{
public:
	Model(const char* filename);

	int get_nof_vertices();
	int get_nof_faces();
	Vec3f get_vertex(int index);
	std::vector<int> get_face(int index);

private:
	std::vector<Vec3f> vertices;
	std::vector<std::vector<int>> faces;
};
