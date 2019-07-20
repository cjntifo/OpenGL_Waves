#pragma once

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "GL_Util.h"

class GeometryGenerator
{
public:

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


	void CreateGrid(float width, float depth, int m, int n, MeshData& meshData);

private:
	/* more to come... */
};

#endif // GEOMETRYGENERATOR_H