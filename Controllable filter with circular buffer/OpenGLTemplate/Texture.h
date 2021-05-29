#pragma once

// Class that provides a texture for texture mapping in OpenGL
class CTexture
{
public:
	void CreateFromData(BYTE* data, int width, int height, int bpp, GLenum format, bool generateMipMaps = false);
	bool Load(string path, bool generateMipMaps = true);
	void Bind(int textureUnit = 0);

	void SetSamplerObjectParameter(GLenum parameter, GLenum value);
	void SetSamplerObjectParameterf(GLenum parameter, float value);

	int GetWidth();
	int GetHeight();
	int GetBPP();

	void Release();

	CTexture();
	~CTexture();
private:
	int m_width, m_height, m_bpp; // Texture width, height, and bytes per pixel
	UINT m_textureID; // Texture id
	UINT m_samplerObjectID; // Sampler id
	bool m_mipMapsGenerated;

	string m_path;
};

