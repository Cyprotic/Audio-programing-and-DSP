#include "VertexBufferObject.h"


// Constructor -- initialise member variable m_bDataUploaded to false
CVertexBufferObject::CVertexBufferObject()
{
	m_dataUploaded = false;
}

CVertexBufferObject::~CVertexBufferObject()
{
}

// Create a VBO 
void CVertexBufferObject::Create()
{
	glGenBuffers(1, &m_vbo);
}

// Release the VBO and any associated data
void CVertexBufferObject::Release()
{
	glDeleteBuffers(1, &m_vbo);
	m_dataUploaded = false;
	m_data.clear();
}


// Binds a VBO.  
void CVertexBufferObject::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}


// Uploads the data to the GPU.  Afterwards, the data can be cleared.  
// iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...
void CVertexBufferObject::UploadDataToGPU(int usageHint)
{
	glBufferData(GL_ARRAY_BUFFER, m_data.size(), &m_data[0], usageHint);
	m_dataUploaded = true;
	m_data.clear();
}

// Adds data to the VBO.  
void CVertexBufferObject::AddData(void* ptrData, UINT dataSize)
{
	m_data.insert(m_data.end(), (BYTE*)ptrData, (BYTE*)ptrData+dataSize);
}



