#pragma once

#include "Texture.h"
#include "vertexBufferObject.h"
#include "./include/glm/gtc/type_ptr.hpp"

class CCubemap
{
public:
	void Create(string sPositiveX, string sNegativeX, string sPositiveY, string sNegativeY, string sPositiveZ, string sNegativeZ);
	void Release();
	bool LoadTexture(string filename, BYTE **bmpBytes, int &iWidth, int &iHeight);
	void Bind(int iTextureUnit = 0);


private:
	UINT m_uiVAO;
	CVertexBufferObject m_vboRenderData;
	GLuint m_uiTexture;
	GLuint m_uiSampler; // Sampler name

};