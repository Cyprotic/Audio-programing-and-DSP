#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

typedef struct {
	std::vector<unsigned int> id;	// list of triangle IDs 
} TriangleList;


class CVertex
{
public:
	CVertex(){};
	CVertex(glm::vec3 positionIn, glm::vec2 textureCoordIn, glm::vec3 normalIn)
	{
		position = positionIn;
		textureCoord = textureCoordIn;
		normal = normalIn;
	};

	glm::vec3 position;
	glm::vec2 textureCoord;
	glm::vec3 normal;
};

class CFaceVertexMesh
{
public:
	CFaceVertexMesh();
	~CFaceVertexMesh();
	void Render();
	bool CreateFromTriangleList(const std::vector<CVertex> &vertices, const std::vector<unsigned int> &triangles);
	void ComputeVertexNormals();
	glm::vec3 ComputeTriangleNormal(unsigned int tId);
	void ComputeTextureCoordsXZ(float xScale, float zScale);


private:
	std::vector<CVertex> m_vertices;			// A list of vertices
	std::vector<unsigned int> m_triangles;		// Stores vertex IDs -- every three makes a triangle
	std::vector<TriangleList> m_onTriangle;	// For each vertex, stores a list of triangle IDs saying which triangles the vertex is on
	UINT m_uiVAO;
};