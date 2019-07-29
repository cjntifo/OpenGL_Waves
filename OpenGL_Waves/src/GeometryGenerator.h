#pragma once

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "GL_Util.h"

class GeometryGenerator
{
public:

	struct Vertex
	{
		Vertex() {}
		Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv)
			: Position(p), Normal(n), TexC(uv) {}
		Vertex(	float px, float py, float pz,
				float nx, float ny, float nz,
				float u, float v)
				: Position(px, py, pz), Normal(nx, ny, nz), TexC(u, v) {}
		
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexC;

		// Additional code allowing for TangentU
		/*Vertex() {}
		Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec3& t, const glm::vec2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 TangentU;
		glm::vec2 TexC;*/
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<GLuint> Indices;
	};

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	void CreateGrid(float width, float depth, int m, int n, MeshData& meshData);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateCylinder(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
	
	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateSphere(float radius, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
	
	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	void CreateGeosphere(float radius, unsigned int numSubdivisions, MeshData& meshData);

	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	void CreateBox(float width, float height, float depth, MeshData& meshData);

private:
	const float PI = 3.14159265;

	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
};

#endif // GEOMETRYGENERATOR_H