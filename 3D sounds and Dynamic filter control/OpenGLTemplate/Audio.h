#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "Game.h"



class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadEventSound(const char *filename);
	bool PlayEventSound();
	bool LoadMusicStream(const char *filename);
	bool PlayMusicStream();
	void Update(glm::vec3 cameraPOS, glm::vec3 cameraStrafe, glm::vec3 cameraView, glm::vec3 carPOS, glm::vec3 cameraUpVector);

private:
		

	void FmodErrorCheck(FMOD_RESULT result);


	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound *m_eventSound;

	
	FMOD::Sound *m_music;
	FMOD::Channel* m_musicChannel;

};
