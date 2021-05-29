#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"

// Class for generating a unit sphere
class CSphere
{
public:
	CSphere();
	~CSphere();
	void Create(string directory, string front, int slicesIn, int stacksIn);
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numTriangles;
};