#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "Camera.h"

class CAudio{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadEventSound(char *filename);
	bool PlayEventSound();
	bool PlayEngineSound(glm::vec3 enginePOS);
	bool LoadMusicStream(char *filename);
	bool PlayMusicStream();
	bool LoadEngineSound(char* filename);
	void ToggleMusicFilter();
	void ToggleHighFilter();
	void ToggleLowFilter();
	void ToggleHalvedFilter();
	void IncreaseMusicVolume();
	void DecreaseMusicVolume();
	void IncreaseEngineVolume();
	void DecreaseEngineVolume();
	void ChangeEngineSound();
	void Update(CCamera *m_pCamera, glm::vec3 pos);

private:
	void FmodErrorCheck(FMOD_RESULT result);
	void ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec);

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	
	FMOD::Sound *m_eventSound;
	FMOD::Channel *m_eventChannel;

	FMOD::Sound *m_engineSound;
	FMOD::Channel *m_engineChannel;

	FMOD::Sound *m_music;
	FMOD::DSP *m_musicFilter;
	FMOD::Channel *m_musicChannel;
	FMOD::DSP *m_musicDSPHead;
	FMOD::DSP *m_musicDSPHeadInput;

	bool m_musicFilterActive;
	bool m_highLowFilterActive;
	float m_musicVolume = 0.2f;
	float m_engineSpeed;


	FMOD::DSP *m_dsp; // For the DSP effect using DSPCallback
	FMOD::DSP* m_dspHigh; // For the DSP effect using DSPCallback
	FMOD::DSP* m_dspLow; // For the DSP effect using DSPCallback
	FMOD_VECTOR camPos;


};
