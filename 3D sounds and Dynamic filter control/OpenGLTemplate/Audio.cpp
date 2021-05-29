#include "Audio.h"
#include "Game.h"

#pragma comment(lib, "lib/fmod_vc.lib")

CAudio::CAudio()
{}

CAudio::~CAudio()
{}

bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	
}

// Load an event sound
bool CAudio::LoadEventSound(const char *filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_3D | FMOD_LOOP_NORMAL, 0, &m_eventSound);
	result = m_FmodSystem->set3DSettings(1.0, 0.5, 1.0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play an event sound
bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &m_musicChannel);
	result = m_musicChannel->setVolume(1.0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(const char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;
	return true;
}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{						
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

void CAudio::Update(glm::vec3 cameraPOS, glm::vec3 cameraStrafe, glm::vec3 cameraView, glm::vec3 carPOS, glm::vec3 cameraUpVector)
{

	FMOD_VECTOR pos, vel;
	pos.x = carPOS.x;
	pos.y = carPOS.y;
	pos.z = carPOS.z;
	vel.x = 10.0f;
	vel.y = 10.0f;
	vel.z = 10.0f;
	m_musicChannel->set3DSpread(360);
	m_musicChannel->set3DMinMaxDistance(1.0, 1000.0);
	m_musicChannel->set3DAttributes(&pos, &vel);	 // The the 3D position of the sound
	FMOD_VECTOR listener_position, listener_velocity, up_vector, view_vector;

	glm::vec3 dir = cameraStrafe;  // note: viewVector pointing backwards due to right-handed coordinate system
	dir = glm::normalize(dir);
	listener_position.x = cameraPOS.x;
	listener_position.y = cameraPOS.y;
	listener_position.z = cameraPOS.z;
	listener_velocity.x = 10;
	listener_velocity.y = 10;
	listener_velocity.z = 10;
	up_vector.x = cameraUpVector.x;
	up_vector.y = cameraUpVector.y;
	up_vector.z = cameraUpVector.z;
	view_vector.x = dir.x;
	view_vector.y = dir.y;
	view_vector.z = dir.z;

	// Update the listener position, velocity, and orientation based on the camera
	m_FmodSystem->set3DListenerAttributes(0, &listener_position, &listener_velocity, &view_vector, &up_vector);
	
	m_FmodSystem->update();
}
