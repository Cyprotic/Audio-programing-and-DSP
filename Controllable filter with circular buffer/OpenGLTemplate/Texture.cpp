#include "Common.h"

#include "texture.h"

#include "include\freeimage\FreeImage.h"
#pragma comment(lib, "lib/FreeImage.lib")

CTexture::CTexture()
{
	m_mipMapsGenerated = false;
}
CTexture::~CTexture()
{}

// Create a texture from the data stored in bData.  
void CTexture::CreateFromData(BYTE* data, int width, int height, int bpp, GLenum format, bool generateMipMaps)
{
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	if(format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	// We must handle this because of internal format parameter
	else if(format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if(generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &m_samplerObjectID);

	m_path = "";
	m_mipMapsGenerated = generateMipMaps;
	m_width = width;
	m_height = height;
	m_bpp = bpp;
}

// Loads a 2D texture given the filename (sPath).  bGenerateMipMaps will generate a mipmapped texture if true
bool CTexture::Load(string path, bool generateMipMaps)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(path.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, path.c_str());

	if(!dib) {
		char message[1024];
		sprintf_s(message, "Cannot load image\n%s\n", path.c_str());
		MessageBox(NULL, message, "Error", MB_ICONERROR);
		return false;
	}

	BYTE* pData = FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if (pData == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;
	

	GLenum format;
	int bada = FreeImage_GetBPP(dib);
	if(FreeImage_GetBPP(dib) == 32)format = GL_BGRA;
	if(FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if(FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	CreateFromData(pData, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, generateMipMaps);
	
	FreeImage_Unload(dib);

	m_path = path;

	return true; // Success
}

void CTexture::SetSamplerObjectParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(m_samplerObjectID, parameter, value);
}

void CTexture::SetSamplerObjectParameterf(GLenum parameter, float value)
{
	glSamplerParameterf(m_samplerObjectID, parameter, value);
}


// Binds a texture for rendering
void CTexture::Bind(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBindSampler(iTextureUnit, m_samplerObjectID);
}

// Frees memory on the GPU of the texture
void CTexture::Release()
{
	glDeleteSamplers(1, &m_samplerObjectID);
	glDeleteTextures(1, &m_textureID);
}

int CTexture::GetWidth()
{
	return m_width;
}

int CTexture::GetHeight()
{
	return m_height;
}

int CTexture::GetBPP()
{
	return m_bpp;
}