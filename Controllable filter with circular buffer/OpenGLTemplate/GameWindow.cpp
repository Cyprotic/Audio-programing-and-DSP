#include "gamewindow.h"

#include "include/gl/glew.h"
#include "include/gl/wglew.h"


#define SIMPLE_OPENGL_CLASS_NAME "simple_openGL_class_name"

GameWindow& GameWindow::GetInstance() 
{
  static GameWindow instance;

  return instance;
}

GameWindow::GameWindow() : m_fullscreen(false) 
{
}

// A message handler for the dummy window
LRESULT CALLBACK MsgHandlerSimpleOpenGLClass(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch(uiMsg)
	{
		case WM_PAINT:									
			BeginPaint(hWnd, &ps);							
			EndPaint(hWnd, &ps);					
			break;

		default:
			return DefWindowProc(hWnd, uiMsg, wParam, lParam); // Default window procedure
	}
	return 0;
}

// A function to register the dummy window
void GameWindow::RegisterSimpleOpenGLClass(HINSTANCE hInstance)
{
	static bool bClassRegistered = false;
	if(bClassRegistered) return;
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC) MsgHandlerSimpleOpenGLClass;
	wc.cbClsExtra = 0; wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENUBAR+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = SIMPLE_OPENGL_CLASS_NAME;

	RegisterClassEx(&wc);

	bClassRegistered = true;
}

// Create a dummy window, intialise GLEW, and then delete the dummy window
bool GameWindow::InitGLEW()
{
	static bool bGlewInitialized = false;
	if(bGlewInitialized) return true;

	RegisterSimpleOpenGLClass(m_hinstance);

	HWND hWndFake = CreateWindow(SIMPLE_OPENGL_CLASS_NAME, "FAKE", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, m_hinstance, NULL);

	m_hdc = GetDC(hWndFake);

	// First, choose false pixel format
	
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
 
	int iPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	if (iPixelFormat == 0)return false;

	if(!SetPixelFormat(m_hdc, iPixelFormat, &pfd))return false;

	// Create the false, old style context (OpenGL 2.1 and before)

	HGLRC hRCFake = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, hRCFake);

	bool bResult = true;

	if(!bGlewInitialized)
	{
		if(glewInit() != GLEW_OK)
		{
			MessageBox(m_hwnd, "Couldn't initialize GLEW!", "Fatal Error", MB_ICONERROR);
			bResult = false;
		}
		bGlewInitialized = true;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCFake);
	DestroyWindow(hWndFake);

	return bResult;
}

// Initialise GLEW and create the real game window
HDC GameWindow::Init(HINSTANCE hinstance) 
{
	m_hinstance = hinstance;
	if(!InitGLEW())
		return false;

	m_appName = "OpenGL";

	CreateGameWindow("OpenGL Template");

	// If we never got a valid window handle, quit the program
	if(m_hwnd == NULL) {
		return NULL;
	} else {
		return m_hdc;
	}
}

// Create the game window
void GameWindow::CreateGameWindow(string sTitle) 
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;

	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	wcex.hIcon = LoadIcon(m_hinstance, IDI_WINLOGO);
	wcex.hIconSm = LoadIcon(m_hinstance, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(m_hinstance, IDC_ARROW);
	wcex.hInstance = m_hinstance;

	wcex.lpfnWndProc = WinProc;
	wcex.lpszClassName = m_appName.c_str();

	wcex.lpszMenuName = NULL;

	RegisterClassEx(&wcex);
	
	
	// Windowed mode.  Uncomment text below to have a choice between windowed mode and full screen mode
	m_hwnd = CreateWindowEx(0, m_appName.c_str(), sTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
								0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, m_hinstance, NULL);
								
								
	/*
 	if(MessageBox(NULL, "Click Yes to go to windowed mode", "Fullscreen", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		m_hwnd = CreateWindowEx(0, m_sAppName.c_str(), sTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
								0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, m_hinstance, NULL);
	} else {
		DEVMODE dmSettings = {0};
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings); // Get current display settings

		m_hwnd = CreateWindowEx(0, m_sAppName.c_str(), sTitle.c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // This is the commonly used style for fullscreen
							0, 0, dmSettings.dmPelsWidth, dmSettings.dmPelsHeight, NULL, NULL, m_hinstance, NULL);
		
	} 
	*/


	// Initialise OpenGL here
	InitOpenGL();
	
	ShowWindow(m_hwnd, SW_SHOW);
	GetClientRect(m_hwnd, &m_dimensions);

	UpdateWindow(m_hwnd);

	ShowCursor(FALSE);
	SetFocus(m_hwnd);
	
}

// Initialise OpenGL, including the pixel format descriptor and the OpenGL version
void GameWindow::InitOpenGL()
{

	m_hdc = GetDC(m_hwnd);

	bool bError = false;
	PIXELFORMATDESCRIPTOR pfd;

	int iMajorVersion = 4;
	int iMinorVersion = 0;

	if(iMajorVersion <= 2)
	{
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;
 
		int iPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
		if (iPixelFormat == 0)return;

		if(!SetPixelFormat(m_hdc, iPixelFormat, &pfd))return;

		// Create the old style context (OpenGL 2.1 and before)
		m_hrc = wglCreateContext(m_hdc);
		if(m_hrc) 
			wglMakeCurrent(m_hdc, m_hrc);
		else bError = true;
	}
	else if(WGLEW_ARB_create_context && WGLEW_ARB_pixel_format)
	{
		const int iPixelFormatAttribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			0 // End of attributes list
		};
		int iContextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, iMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, iMinorVersion,
		    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0 // End of attributes list
		};

		int iPixelFormat, iNumFormats;
		wglChoosePixelFormatARB(m_hdc, iPixelFormatAttribList, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);

		// PFD seems to be only redundant parameter now
		if(!SetPixelFormat(m_hdc, iPixelFormat, &pfd))return;

		m_hrc = wglCreateContextAttribsARB(m_hdc, 0, iContextAttribs);
		// If everything went OK
		if(m_hrc) wglMakeCurrent(m_hdc, m_hrc);
		else bError = true;

	}
	else bError = true;
	
	if(bError)
	{
		// Generate error messages
		char sErrorMessage[255], sErrorTitle[255];
		sprintf_s(sErrorMessage, "OpenGL %d.%d is not supported! Please download latest GPU drivers and check your graphics card capability!", iMajorVersion, iMinorVersion);
		sprintf_s(sErrorTitle, "OpenGL %d.%d Not Supported", iMajorVersion, iMinorVersion);
		MessageBox(m_hwnd, sErrorMessage, sErrorTitle, MB_ICONINFORMATION);
		return;
	}


	return;
}

// Deinitialise the window and rendering context
void GameWindow::Deinit()
{
	if (m_hrc) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hrc);
	}

	if (m_hdc) {
		ReleaseDC(m_hwnd, m_hdc);
	}

	if (m_fullscreen) {
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	UnregisterClass(m_class, m_hinstance);
	PostQuitMessage(0);
}