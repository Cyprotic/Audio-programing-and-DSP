#pragma once

#include <windows.h>
#include "Common.h"

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam);

class GameWindow {
public:
	static GameWindow& GetInstance();
	GameWindow();

	enum {
		SCREEN_WIDTH = 800,
		SCREEN_HEIGHT = 600,
	};

	HDC Init(HINSTANCE hinstance);
	void Deinit();

	void SetDimensions(RECT dimensions) {m_dimensions = dimensions;}
	RECT GetDimensions() {return m_dimensions;}

	bool Fullscreen() const { return m_fullscreen; }

	HDC Hdc() const { return m_hdc; }
	HINSTANCE Hinstance() const { return m_hinstance; }
	HGLRC Hrc() const { return m_hrc; }
	HWND  Hwnd() const { return m_hwnd; }

private:
	
	GameWindow(const GameWindow&);
	void operator=(const GameWindow&);

	void CreateGameWindow(string title);
	void InitOpenGL();
	bool InitGLEW();
	void RegisterSimpleOpenGLClass(HINSTANCE hInstance);

	bool  m_fullscreen;

	HDC   m_hdc;
	HINSTANCE m_hinstance;
	HGLRC m_hrc;
	HWND  m_hwnd;

	LPSTR m_class;
	RECT  m_dimensions;

	string m_appName;

};

