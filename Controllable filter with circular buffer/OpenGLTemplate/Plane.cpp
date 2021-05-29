#include "Common.h"
#include "Plane.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CPlane::CPlane()
{}

CPlane::~CPlane()
{}


// Create the plane, including its geometry, texture mapping, normal, and colour
void CPlane::Create(string directory, string filename, float width, float height, float textureRepeat)
{
	
	m_width = width;
	m_height = height;

	// Load the texture
	m_texture.Load(directory+filename, true);

	m_directory = directory;
	m_filename = filename;

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();

	float halfWidth = m_width / 2.0f;
	float halfHeight = m_height / 2.0f;

	// Vertex positions
	glm::vec3 planeVertices[4] = 
	{
		glm::vec3(-halfWidth, 0.0f, -halfHeight), 
		glm::vec3(-halfWidth, 0.0f, halfHeight), 
		glm::vec3(halfWidth, 0.0f, -halfHeight), 
		glm::vec3(halfWidth, 0.0f, halfHeight), 
	};

	// Texture coordinates
	glm::vec2 planeTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f), 
		glm::vec2(0.0f, textureRepeat), 
		glm::vec2(textureRepeat, 0.0f), 
		glm::vec2(textureRepeat, textureRepeat)
	};

	// Plane normal
	glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 4; i++) {
		m_vbo.AddData(&planeVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&planeTexCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&planeNormal, sizeof(glm::vec3));
	}


	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
	
}

// Render the plane as a triangle strip
void CPlane::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
}

// Release resources
void CPlane::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}