#pragma once

#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"

class CCamera {
public:
	CCamera();										// Constructor - sets default values for camera position, viewvector, upvector, and speed
	~CCamera();										// Destructor

	glm::vec3 GetPosition() const;					// Gets the position of the camera centre of projection
	glm::vec3 GetView() const;						// Gets the position of the camera view point
	glm::vec3 GetUpVector() const;					// Gets the camera up vector
	glm::vec3 GetStrafeVector() const;				// Gets the camera strafe vector
	glm::mat4* GetPerspectiveProjectionMatrix();	// Gets the camera perspective projection matrix
	glm::mat4* GetOrthographicProjectionMatrix();	// Gets the camera orthographic projection matrix
	glm::mat4 GetViewMatrix();						// Gets the camera view matrix - note this is not stored in the class but returned using glm::lookAt() in GetViewMatrix()

	// Set the camera position, viewpoint, and up vector
	void Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector);
	
	// Rotate the camera viewpoint -- this effectively rotates the camera
	void RotateViewPoint(float angle, glm::vec3 &viewPoint);

	// Respond to mouse movement to rotate the camera
	void SetViewByMouse();

	// Respond to keyboard presses on arrow keys to translate the camera
	void TranslateByKeyboard(double dt);

	// Strafe the camera (move it side to side)
	void Strafe(double direction);

	// Advance the camera (move it forward or backward)
	void Advance(double direction);

	// Update the camera
	void Update(double dt);

	// Set the projection matrices
	void SetPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane);
	void SetOrthographicProjectionMatrix(int width, int height);

	glm::mat3 ComputeNormalMatrix(const glm::mat4 &modelViewMatrix);

private:
	glm::vec3 m_position;			// The position of the camera's centre of projection
	glm::vec3 m_view;				// The camera's viewpoint (point where the camera is looking)
	glm::vec3 m_upVector;			// The camera's up vector
	glm::vec3 m_strafeVector;		// The camera's strafe vector

	float m_speed;					// How fast the camera moves

	glm::mat4 m_perspectiveProjectionMatrix;		// Perspective projection matrix
	glm::mat4 m_orthographicProjectionMatrix;		// Orthographic projection matrix
};
