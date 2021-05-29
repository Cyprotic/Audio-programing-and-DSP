#include "Common.h"
#include "pyramid.h"

pyramid::pyramid() {}

pyramid::~pyramid() {}


// Create the plane, including its geometry, texture mapping, normal, and colour
void pyramid::Create(string directory, string filename, float width, float height, float length, float textureRepeat)
{

	m_width = width;
	m_height = height;
	m_length = length;

	// Load the texture
	m_texture.Load(directory + filename, true);

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
	float halfLength = m_length / 2.0f;

	// Vertex positions
	glm::vec3 cVertices[18] =
	{
		// Front face
		glm::vec3(0.0f, m_height, 0.0f),
		glm::vec3(-halfWidth, 0.0f, halfLength),
		glm::vec3(halfWidth, 0.0f, halfLength),

		// Back face
		glm::vec3(0.0f, m_height, 0.0f),
		glm::vec3(halfWidth, 0.0f, -halfLength),
		glm::vec3(-halfWidth, 0.0f, -halfLength),

		// Right side
		glm::vec3(0.0f, m_height, 0.0f),
		glm::vec3(halfWidth, 0.0f, halfLength),
		glm::vec3(halfWidth, 0.0f, -halfLength),

		// Left side
		glm::vec3(0.0f, m_height, 0.0f),
		glm::vec3(-halfWidth, 0.0f, -halfLength),
		glm::vec3(-halfWidth, 0.0f, halfLength),

		// Bottom
		glm::vec3(-halfWidth, 0.0f, halfLength),
		glm::vec3(halfWidth, 0.0f, halfLength),
		glm::vec3(-halfWidth, 0.0f, -halfLength),

		glm::vec3(halfWidth, 0.0f, halfLength),
		glm::vec3(halfWidth, 0.0f, -halfLength),
		glm::vec3(-halfWidth, 0.0f, -halfLength)

	};

	// Texture coordinates
	glm::vec2 TexCoords[18] =
	{
		// Front face
		glm::vec2(0.5f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// Back face
		glm::vec2(0.5f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// Right Side
		glm::vec2(0.5f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// Left side
		glm::vec2(0.5f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// Bottom
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),	

	};

	// face normals
	glm::vec3 normals[18] =
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),

		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),

		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),

		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),

		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),

		// fix bottom

	};

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 18; i++) {
		m_vbo.AddData(&cVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&TexCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&normals[i], sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations (interleaved)
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the plane as a triangle strip
void pyramid::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 18);

}

// Release resources
void pyramid::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}