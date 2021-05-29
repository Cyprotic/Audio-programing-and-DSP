#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include <vector> 



class CircularBuffer
{
public:
	CircularBuffer();
	~CircularBuffer();
	bool Initialise();
	bool LoadEventSound(char* filename);
	bool PlayEventSound();
	bool LoadMusicStream(char* filename);
	bool PlayMusicStream();
	void ToggleMusicFilter();
	void Update();

private:
	std::vector<int> buffer;
	int writer;
	int value;
	int buff_size;
};
