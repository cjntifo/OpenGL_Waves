#pragma once

#ifndef GLUTIL_H
#define GLUTIL_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct vertex
{
	GLfloat x, y, z;
	GLfloat a, b, c;
	GLfloat u, v;
};

struct normal
{
	GLfloat x, y, z;
};

struct index
{
	GLuint a, b, c;
};

struct MeshData
{
	std::vector<vertex> Vertices;
	//std::vector<normal> Normals;
	std::vector<index> Indices;
};

#endif //GLUTIL_H