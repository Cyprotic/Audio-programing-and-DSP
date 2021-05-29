#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Common.h"
#include "Texture.h"
#include "Shaders.h"
#include "VertexBufferObject.h"


// This class is a wrapper for FreeType fonts and their usage with OpenGL
class CFreeTypeFont
{
public:
	CFreeTypeFont();
	~CFreeTypeFont();

	bool LoadFont(string file, int pixelSize);
	bool LoadSystemFont(string name, int pixelSize);

	int GetTextWidth(string text, int pixelSize);

	void Print(string text, int x, int y, int pixelSize = -1);
	void Render(int x, int y, int pixelSize, char* text, ...);

	
	void ReleaseFont();

	void SetShaderProgram(CShaderProgram* shaderProgram);

private:
	void CreateChar(int index);

	CTexture m_charTextures[256];
	int m_advX[256], m_advY[256];
	int m_bearingX[256], m_bearingY[256];
	int m_charWidth[256], m_charHeight[256];
	int m_loadedPixelSize, m_newLine;

	bool m_isLoaded;

	UINT m_vao;
	CVertexBufferObject m_vbo;

	FT_Library m_ftLib;
	FT_Face m_ftFace;
	CShaderProgram* m_shaderProgram;
};
