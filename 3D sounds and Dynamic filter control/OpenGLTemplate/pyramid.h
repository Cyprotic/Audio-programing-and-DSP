#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a xz plane of a given size
class pyramid
{
public:
	pyramid();
	~pyramid();
	void Create(string sDirectory, string sFilename, float fWidth, float fHeight, float length, float fTextureRepeat);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	float m_width;
	float m_height;
	float m_length;
};