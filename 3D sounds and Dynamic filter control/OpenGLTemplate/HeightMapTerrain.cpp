#include "HeightMapTerrain.h"
#pragma comment(lib, "lib/FreeImage.lib")


CHeightMapTerrain::CHeightMapTerrain()
{
	m_dib = NULL;
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	delete [] m_heightMap;
}

// Convert a point from image (pixel) coordinates to world coordinates
glm::vec3 CHeightMapTerrain::ImageToWorldCoordinates(glm::vec3 p)
{
	// Normalize the image point so that it in the range [-1, 1] in x and [-1, 1] in z
	p.x = 2.0f * (p.x / m_width) - 1.0f;
	p.z = 2.0f * (p.z / m_height) - 1.0f;

	// Now scale the point so that the terrain has the right size in x and z
	p.x *= m_terrainSizeX / 2.0f;
	p.z *= m_terrainSizeZ / 2.0f;

	// Now translate the point based on the origin passed into the function
	p += m_origin;

	return p;

}

// Convert a point from world coordinates to image (pixel) coordinates
glm::vec3 CHeightMapTerrain::WorldToImageCoordinates(glm::vec3 p)
{
	p -= m_origin;

	// Normalize the image point so that it in the range [-1, 1] in x and [-1, 1] in z
	p.x *= 2.0f / m_terrainSizeX;
	p.z *= 2.0f / m_terrainSizeZ;

	// Now transform the point so that it is in the range [0, 1] in x and [0, 1] in z
	p.x = (p.x + 1.0f) * (m_width / 2.0f);
	p.z = (p.z + 1.0f) * (m_height / 2.0f);

	return p;

}

bool CHeightMapTerrain::GetImageBytes(const char *terrainFilename, BYTE **bDataPointer, unsigned int &width, unsigned int &height)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
	fif = FreeImage_GetFileType(terrainFilename, 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(terrainFilename);
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		m_dib = FreeImage_Load(fif, terrainFilename);

	if(!m_dib) {
		char message[1024];
		sprintf_s(message, "Cannot load image\n%s\n", terrainFilename);
		MessageBox(NULL, message, "Error", MB_ICONERROR);
		return false;
	}

	*bDataPointer = FreeImage_GetBits(m_dib); // Retrieve the image data
	width = FreeImage_GetWidth(m_dib);
	height = FreeImage_GetHeight(m_dib);

	// If somehow one of these failed (they shouldn't), return failure
	if(bDataPointer == NULL || width == 0 || height == 0) 
		return false;

	return true;
}

// This function generates a heightmap terrain based on a bitmap
bool CHeightMapTerrain::Create(const char *terrainFilename, const char *textureFilename, glm::vec3 origin, float terrainSizeX, float terrainSizeZ, float terrainHeightScale)
{
	BYTE *bDataPointer;
	unsigned int width, height;

	if (GetImageBytes(terrainFilename, &bDataPointer, width, height) == false)
		return false;

	m_width = width;
	m_height = height;
	m_origin = origin;
	m_terrainSizeX = terrainSizeX;
	m_terrainSizeZ = terrainSizeZ;
	
	// Allocate memory and initialize to store the image
	m_heightMap = new float[m_width * m_height];
	if (m_heightMap == NULL) 
		return false;

	// Clear the heightmap
	memset(m_heightMap, 0, m_width * m_height * sizeof(float));

	// Form mesh
	std::vector<CVertex> vertices;
	std::vector<unsigned int> triangles;
	float halfSizeX = m_width / 2.0f;
	float halfSizeY = m_height / 2.0f;
	
	int X = 1;
	int Z = m_width;
	int triangleId = 0;
	for (int z = 0; z < m_height; z++) {
		for (int x = 0; x < m_width; x++) {
			int index = x + z * m_width;

			// Retreive the colour from the terrain image, and set the normalized height in the range [0, 1]
			float grayScale = (bDataPointer[index*3] + bDataPointer[index*3+1] + bDataPointer[index*3+2]) / 3.0f;
			float height = (grayScale - 128.0f) / 128.0f;

			// Make a point based on this pixel position.  Then, transform so that the mesh has the correct size and origin
			// This transforms a point in image coordinates to world coordinates
			glm::vec3 pImage = glm::vec3((float) x, height, (float) z);
			glm::vec3 pWorld = ImageToWorldCoordinates(pImage);

			// Scale the terrain and store for later
			pWorld.y *= terrainHeightScale;	 
			m_heightMap[index] = pWorld.y;

			// Store the point in a vector
			CVertex v = CVertex(pWorld, glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
			vertices.push_back(v);
		}
	}

	FreeImage_Unload(m_dib);

	// Form triangles from successive rows of the image
	for (int z = 0; z < m_height-1; z++) {
		for (int x = 0; x < m_width-1; x++) {
			int index = x + z * m_width;
			triangles.push_back(index);
			triangles.push_back(index+X+Z);
			triangles.push_back(index+X);

			triangles.push_back(index);
			triangles.push_back(index+Z);
			triangles.push_back(index+X+Z);
		}
	}

	// Create a face vertex mesh
	m_mesh.CreateFromTriangleList(vertices, triangles);

	// Load a texture for texture mapping the mesh
	m_texture.Load(textureFilename, true); 



	return true;
}
// For a point p in world coordinates, return the height of the terrain
float CHeightMapTerrain::ReturnGroundHeight(glm::vec3 p)
{
	// Undo the transformation going from image coordinates to world coordinates
	glm::vec3 pImage = WorldToImageCoordinates(p);
	// Bilinear interpolation. 
	int xl = (int) floor(pImage.x);
	int zl = (int) floor (pImage.z);
	// Check if the position is in the region of the heightmap
	if (xl < 0 || xl >= m_width - 1 || zl < 0 || zl >= m_height -1)
		return 0.0f;
	// Get the indices of four pixels around the current point 
	int indexll = xl + zl * m_width;
	int indexlr = (xl+1) + zl * m_width;
	int indexul = xl + (zl+1) * m_width;
	int indexur = (xl+1) + (zl+1) * m_width;
	// Interpolation amounts in x and z
	float dx = pImage.x - xl;
	float dz = pImage.z - zl;
	// Interpolate -- first in x and and then in z
	float a = (1-dx) * m_heightMap[indexll] + dx * m_heightMap[indexlr];
	float b = (1-dx) * m_heightMap[indexul] + dx * m_heightMap[indexur];
	float c = (1-dz) * a + dz * b;
	return c;
}

void CHeightMapTerrain::Render()
{
	m_texture.Bind();
	m_mesh.Render();
}