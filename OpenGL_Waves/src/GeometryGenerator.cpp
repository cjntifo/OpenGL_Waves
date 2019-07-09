#pragma once

#include "GeometryGenerator.h"

void GeometryGenerator::CreateGrid(float width, float depth, int m, int n, MeshData& meshData) //Based off GeometryGenerator class
{
	int vertexCount = m * n;
	int faceCount = (m - 1)*(n - 1) * 2;
	// Create the vertices.

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);


	meshData.Vertices.resize(vertexCount);
	for (GLfloat i = 0.0f; i < m; ++i)
	{
		GLfloat z = halfDepth - i * dz;
		for (GLfloat j = 0.0f; j < n; ++j)
		{
			//code
			GLfloat x = -halfWidth + j * dx;

			meshData.Vertices[i*n + j].x = x;
			meshData.Vertices[i*n + j].y = -3.0f;
			meshData.Vertices[i*n + j].z = z;

			meshData.Vertices[i*n + j].a = 0.0f;
			meshData.Vertices[i*n + j].b = 1.0f;
			meshData.Vertices[i*n + j].c = 0.0f;


			meshData.Vertices[i*n + j].u = j * du;
			meshData.Vertices[i*n + j].v = i * dv;

			//vertex temp = {x, 0.0f, z, j*du, i*dv};
			//meshData.Vertices.push_back(temp);
		}
	}

	//indices
	//meshData.Indices.resize(faceCount);
	GLuint offset = 0;
	for (GLuint i = 0; i < m - 1; ++i)
	{
		for (GLuint j = 0; j < n - 1; ++j)
		{
			//offset = x * m;
			index temp =
			{
				i*n + j,
				i*n + j + 1,
				(i + 1)*n + j
			};

			index temp_2 =
			{
				(i + 1)*n + j,
				i*n + j + 1,
				(i + 1)*n + j + 1
			};

			meshData.Indices.push_back(temp);
			meshData.Indices.push_back(temp_2);
		}
	}
}