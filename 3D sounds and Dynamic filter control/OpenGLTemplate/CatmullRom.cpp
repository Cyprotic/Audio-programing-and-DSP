#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	m_controlPoints.push_back(glm::vec3(0, 0.1f, 200));
	m_controlPoints.push_back(glm::vec3(0, 0.1f, 0));
	m_controlPoints.push_back(glm::vec3(0, 0.1f, -200));
	m_controlPoints.push_back(glm::vec3(100, 0.1f, -200));
	m_controlPoints.push_back(glm::vec3(100, 0.1f, -500));
	m_controlPoints.push_back(glm::vec3(600, 0.1f, -650));
	m_controlPoints.push_back(glm::vec3(900, 0.1f, -750));
	m_controlPoints.push_back(glm::vec3(800, 0.1f, -500));
	m_controlPoints.push_back(glm::vec3(650, 0.1f, -350));
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points

	// Call UniformlySampleControlPoints with the number of samples required

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card

	// Get the control points into the m_controlpoints
	SetControlPoints();

	// Sample the control points to create points 500 on the centreline
	UniformlySampleControlPoints(300);

	glGenVertexArrays(1, &m_vaoCentreline);
    glBindVertexArray(m_vaoCentreline);

	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < m_controlPoints.size(); i++) 
	{
		vbo.AddData(&m_centrelinePoints.at(i), sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	
}

void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	for (int i = 0; i < m_centrelinePoints.size(); ++i)
	{
		if (i == m_centrelinePoints.size() - 1)
		{
			glm::vec3 T = glm::normalize(m_centrelinePoints[0] - m_centrelinePoints[i]);
			glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 B = glm::normalize(glm::cross(N, T));
			m_leftOffsetPoints.push_back(m_centrelinePoints[i] - (m_path_width / 2) * N);
			m_rightOffsetPoints.push_back(m_centrelinePoints[i] + (m_path_width / 2) * N);
		}
		else
		{
			glm::vec3 T = glm::normalize(m_centrelinePoints[i + 1] - m_centrelinePoints[i]);
			glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 B = glm::normalize(glm::cross(N, T));
			m_leftOffsetPoints.push_back(m_centrelinePoints[i] - (m_path_width / 2) * N);
			m_rightOffsetPoints.push_back(m_centrelinePoints[i] + (m_path_width / 2) * N);

		}
	}

	// VAO-VBO LEFT OFFSET CURVE
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo_left;
	vbo_left.Create();
	vbo_left.Bind();
	glm::vec2 texCoordleft(0.0f, 0.0f);
	glm::vec3 normalleft(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_leftOffsetPoints.size(); i++) {
		vbo_left.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		vbo_left.AddData(&texCoordleft, sizeof(glm::vec2));
		vbo_left.AddData(&normalleft, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo_left.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei strideleft = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideleft, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideleft, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strideleft, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// VAO & VBO RIGHT OFFSET CURVE
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo_right;
	vbo_right.Create();
	vbo_right.Bind();
	glm::vec2 texCoordright(0.0f, 0.0f);
	glm::vec3 normalright(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++) {
		vbo_right.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbo_right.AddData(&texCoordright, sizeof(glm::vec2));
		vbo_right.AddData(&normalright, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo_right.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei strideright = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideright, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideright, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strideright, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::CreateTrack()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	glm::vec2 tex1(0.0f, 0.0f);
	glm::vec2 tex2(1.0f, 0.0f);
	glm::vec2 tex3(0.0f, 1.0f);
	glm::vec2 tex4(1.0f, 1.0f);

	
	for (int i = 0; i < m_centrelinePoints.size(); ++i)
	{
		if (i == m_centrelinePoints.size() - 1)
		{
			// Create a VBO
			//Triangle 0-1-2
			vbo.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 1-3-2
			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 2-3-4
			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 3-5-4
			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

		}
		else
		{
			// Create a VBO
			//Triangle 0-1-2
			vbo.AddData(&m_leftOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 1-3-2
			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 2-3-4
			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle 3-5-4
			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;
		}
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride_right = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_right, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_right, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_right, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it

	glLineWidth(2.0f);
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it

	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it

	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());

}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	GL_CULL_FACE;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_texture.Load("resources\\textures\\floor1.jpg");   //https://www.deviantart.com/sangheili456/art/Sci-Fi-Roof-397929348
	
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);