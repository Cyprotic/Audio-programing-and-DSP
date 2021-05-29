#include "Common.h"
#include "cube.h"
//INCLUDE - WHO COMES FIRST ACTUALLY MATTERS WOW.

Cube::Cube()
{}

Cube::~Cube()
{}


// Create the plane, including its geometry, texture mapping, normal, and colour
void Cube::Create(string directory, string filename, float width, float height, float depth, float textureRepeat)
{

	m_width = width;
	m_height = height;
	m_depth = depth;

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
	float halfHeight = m_height / 2.0f;
	float halfLength = m_depth / 2.0f;

	// Vertex positions
	glm::vec3 cVertices[8] =
	{
		// Front face
		glm::vec3(-halfWidth, -halfHeight, halfLength),
		glm::vec3(halfWidth, -halfHeight, halfLength),
		glm::vec3(halfWidth, halfHeight, halfLength),
		glm::vec3(-halfWidth, halfHeight, halfLength),

		// Back face
		glm::vec3(-halfWidth, -halfHeight, -halfLength),
		glm::vec3(halfWidth, -halfHeight, -halfLength),
		glm::vec3(halfWidth, halfHeight, -halfLength),
		glm::vec3(-halfWidth, halfHeight, -halfLength)
	};

	// Texture coordinates
	glm::vec2 TexCoords[36] =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(1.0, 0.0f),

	};

	// Cube Elements
	GLuint cubeIndexes[] =
	{
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 8; i++) {
		m_vbo.AddVertexData(&cVertices[i], sizeof(glm::vec3));

	}
	for (unsigned int i = 0; i < 36; i++) {
		m_vbo.AddVertexData(&TexCoords[i], sizeof(glm::vec2));
		m_vbo.AddIndexData(&cubeIndexes[i], sizeof(GLuint));
	}

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)sizeof(glm::vec3));

}

// Render the plane as a triangle strip
void Cube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

// Release resources
void Cube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}