#include "FreeTypeFont.h"
#include <minmax.h>

#pragma comment(lib, "lib/freetype.lib")

CFreeTypeFont::CFreeTypeFont()
{
	m_isLoaded = false;
}
CFreeTypeFont::~CFreeTypeFont()
{}

/*-----------------------------------------------

Name:	createChar

Params:	iIndex - character index in Unicode.

Result:	Creates one single character (its
		texture).

/*---------------------------------------------*/

inline int next_p2(int n){int res = 1; while(res < n)res <<= 1; return res;}

void CFreeTypeFont::CreateChar(int index)
{
	FT_Load_Glyph(m_ftFace, FT_Get_Char_Index(m_ftFace, index), FT_LOAD_DEFAULT);

	FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &m_ftFace->glyph->bitmap;

	int iW = pBitmap->width, iH = pBitmap->rows;
	int iTW = next_p2(iW), iTH = next_p2(iH);

	GLubyte* bData = new GLubyte[iTW*iTH];
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch < iTH; ch++) 
		for (int cw = 0; cw < iTW; cw++)
			bData[ch*iTW+cw] = (ch >= iH || cw >= iW) ? 0 : pBitmap->buffer[(iH-ch-1)*iW+cw];
 
	// And create a texture from it

	m_charTextures[index].CreateFromData(bData, iTW, iTH, 8, GL_DEPTH_COMPONENT, false);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	m_advX[index] = m_ftFace->glyph->advance.x>>6;
	m_bearingX[index] = m_ftFace->glyph->metrics.horiBearingX>>6;
	m_charWidth[index] = m_ftFace->glyph->metrics.width>>6;

	m_advY[index] = (m_ftFace->glyph->metrics.height - m_ftFace->glyph->metrics.horiBearingY)>>6;
	m_bearingY[index] = m_ftFace->glyph->metrics.horiBearingY>>6;
	m_charHeight[index] = m_ftFace->glyph->metrics.height>>6;

	m_newLine = max(m_newLine, int(m_ftFace->glyph->metrics.height >> 6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec2 vQuad[] =
	{
		glm::vec2(0.0f, float(-m_advY[index]+iTH)),
		glm::vec2(0.0f, float(-m_advY[index])),
		glm::vec2(float(iTW), float(-m_advY[index]+iTH)),
		glm::vec2(float(iTW), float(-m_advY[index]))
	};
	glm::vec2 vTexQuad[] = {glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)};

	// Add this char to VBO
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&vQuad[i], sizeof(glm::vec2));
		m_vbo.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}
	delete[] bData;
}


// Loads an entire font with the given path sFile and pixel size iPXSize
bool CFreeTypeFont::LoadFont(string file, int ipixelSize)
{
	BOOL bError = FT_Init_FreeType(&m_ftLib);
	
	bError = FT_New_Face(m_ftLib, file.c_str(), 0, &m_ftFace);
	if(bError) {
		char message[1024];
		sprintf_s(message, "Cannot load font\n%s\n", file.c_str());
		MessageBox(NULL, message, "Error", MB_ICONERROR);
		return false;
	}
	FT_Set_Pixel_Sizes(m_ftFace, ipixelSize, ipixelSize);
	m_loadedPixelSize = ipixelSize;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();

	for (int i = 0; i < 128; i++)
		CreateChar(i);
	m_isLoaded = true;

	FT_Done_Face(m_ftFace);
	FT_Done_FreeType(m_ftLib);
	
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, (void*)(sizeof(glm::vec2)));
	return true;
}

// Loads a system font with given name (sName) and pixel size (iPXSize)
bool CFreeTypeFont::LoadSystemFont(string name, int ipixelSize)
{
	char buf[512]; GetWindowsDirectory(buf, 512);
	string sPath = buf;
	sPath += "\\Fonts\\";
	sPath += name;

	return LoadFont(sPath, ipixelSize);
}


// Prints text at the specified location (x, y) with the given pixel size (iPXSize)
void CFreeTypeFont::Print(string text, int x, int y, int pixelSize)
{
	if(!m_isLoaded)
		return;

	glBindVertexArray(m_vao);
	m_shaderProgram->SetUniform("sampler0", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if (pixelSize == -1)
		pixelSize = m_loadedPixelSize;
	float fScale = float(pixelSize) / float(m_loadedPixelSize);
	for (int i = 0; i < (int) text.size(); i++) {
		if (text[i] == '\n')
		{
			iCurX = x;
			iCurY -= m_newLine*pixelSize / m_loadedPixelSize;
			continue;
		}
		int iIndex = int(text[i]);
		iCurX += m_bearingX[iIndex] * pixelSize / m_loadedPixelSize;
		if(text[i] != ' ')
		{
			m_charTextures[iIndex].Bind();
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX), float(iCurY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(fScale));
			m_shaderProgram->SetUniform("matrices.modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, iIndex*4, 4);
		}

		iCurX += (m_advX[iIndex] - m_bearingX[iIndex])*pixelSize / m_loadedPixelSize;
	}
	glDisable(GL_BLEND);
}


// Print formatted text at the location (x, y) with specified pixel size (iPXSize)
void CFreeTypeFont::Render(int x, int y, int pixelSize, const char* text, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, text);
	vsprintf_s(buf, text, ap);
	va_end(ap);
	Print(buf, x, y, pixelSize);
}

// Deletes all font textures
void CFreeTypeFont::ReleaseFont()
{
	for (int i = 0; i < 128; i++) 
		m_charTextures[i].Release();
	m_vbo.Release();
	glDeleteVertexArrays(1, &m_vao);
}

// Gets the width of text
int CFreeTypeFont::GetTextWidth(string sText, int iPixelSize)
{
	int iResult = 0;
	for (int i = 0; i < (int)sText.size(); i++)
		iResult += m_advX[sText[i]];
	return iResult*iPixelSize / m_loadedPixelSize;
}

// Sets shader programme that font uses
void CFreeTypeFont::SetShaderProgram(CShaderProgram* shaderProgram)
{
	m_shaderProgram = shaderProgram;
}