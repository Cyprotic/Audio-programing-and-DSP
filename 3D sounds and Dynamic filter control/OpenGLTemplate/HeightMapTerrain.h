#pragma once

#include "Common.h"
#include "FaceVertexMesh.h"
#include "include\freeimage\FreeImage.h"

class CHeightMapTerrain
{
public:
	CHeightMapTerrain();
	~CHeightMapTerrain();
	bool Create(const char *terrainFilename, const char *textureFilename, glm::vec3 origin, float terrainSizeX, float terrainSizeZ, float terrainHeightScale);
	float ReturnGroundHeight(glm::vec3 p);
	void Render();

private:
	int m_width, m_height;
	float *m_heightMap;
	CFaceVertexMesh m_mesh;
	UINT m_hTexture;
	float m_terrainSizeX, m_terrainSizeZ;
	glm::vec3 m_origin;
	CTexture m_texture;
	FIBITMAP* m_dib;

	glm::vec3 WorldToImageCoordinates(glm::vec3 p);
	glm::vec3 ImageToWorldCoordinates(glm::vec3 p);
	bool GetImageBytes(const char *terrainFilename, BYTE **bDataPointer, unsigned int &width, unsigned int &height);
};