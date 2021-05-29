/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "cube.h"
#include "pyramid.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "HeightMapTerrain.h";

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pCarMesh = NULL;
	m_pGateMesh = NULL;
	m_pLampMesh = NULL;
	m_pFenceMesh = NULL;
	m_pHorseMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_pHeightMapTerrain = NULL;
	m_pBox = NULL;
	m_pPyramid = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	cameraSelect = 0.0f;
	goFoward = 0.1f;
	carTurnRight = 0;
	carTurnLeft = 0;
	fogEnable = 0;
	blinkEnable = 0;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pBox;
	delete m_pPyramid;
	delete m_pFtFont;
	delete m_pCarMesh;
	delete m_pGateMesh;
	delete m_pLampMesh;
	delete m_pFenceMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_pHeightMapTerrain;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	m_pModelMatrix = new glm::mat4(1);
	m_pViewMatrix = new glm::mat4(1);
	m_pProjectionMatrix = new glm::mat4(1);
	// This sets the position, viewpoint, and up vector of the camera
	glm::vec3 vEye(10, 10, 10);
	glm::vec3 vView(0, 0, 0);
	glm::vec3 vUp(0, 1, 0);

	*m_pViewMatrix = glm::lookAt(vEye, vView, vUp);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pBox = new Cube;
	m_pPyramid = new pyramid;
	m_pFtFont = new CFreeTypeFont;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pGateMesh = new COpenAssetImportMesh;
	m_pLampMesh = new COpenAssetImportMesh;
	m_pFenceMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_pHeightMapTerrain = new CHeightMapTerrain;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("sphereShader.vert");
	sShaderFileNames.push_back("sphereShader.frag");



	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create the sphere shader program
	CShaderProgram* pSphereProgram = new CShaderProgram;
	pSphereProgram->CreateProgram();
	pSphereProgram->AddShaderToProgram(&shShaders[4]);
	pSphereProgram->AddShaderToProgram(&shShaders[5]);
	pSphereProgram->LinkProgram();
	m_pShaderPrograms->push_back(pSphereProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	//m_pPlanarTerrain->Create("resources\\textures\\", "653_clouds_cloudblock.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	// Create the heightmap terrain
	m_pHeightMapTerrain->Create("resources\\textures\\terrainHeightMap200.bmp", "resources\\textures\\653_clouds_cloudblock.bmp", glm::vec3(0, 0, 0), 2000.0f, 2000.0f, 0.f);

	m_pBox->Create("resources\\textures\\", "white.jpg", 5, 5, 5, 20);
	m_pPyramid->Create("resources\\textures\\", "golden.jpg", 5, 5, 5, 20);

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pCarMesh->Load("resources\\Car\\11675_Car_v1_L3.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pFenceMesh->Load("resources\\Fence\\Road Barrier 01a.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pGateMesh->Load("resources\\gate\\OldTorii.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pLampMesh->Load("resources\\lamp\\streetlamp.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\engine.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	m_pAudio->PlayEventSound();


	// Centerline
	m_pCatmullRom -> CreateCentreline();
	m_pCatmullRom -> CreateOffsetCurves();

}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	vector<glm::vec3> offsetLeft = m_pCatmullRom->m_leftOffsetPoints;
	vector<glm::vec3> offsetRight = m_pCatmullRom->m_rightOffsetPoints;
	vector<glm::vec3> centerPoints = m_pCatmullRom->m_centrelinePoints;

	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("fogEnable", fogEnable);
	pMainProgram->SetUniform("blinkEnable", blinkEnable);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

	// Set up the T for the blinking
	//*m_pModelMatrix = glm::translate(glm::mat4(1), glm::vec3(1, 1, 1));
	pMainProgram->SetUniform("modelMatrix", m_pModelMatrix);
	pMainProgram->SetUniform("t", (float)m_pHighResolutionTimer->Elapsed());
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(offsetLeft[0].x, 15, offsetLeft[0].z, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	pMainProgram->SetUniform("light1.direction",glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));		// Direction material property
	pMainProgram->SetUniform("light1.exponent", 20.0f);		// Exponent material property
	pMainProgram->SetUniform("light1.cutoff", 30.0f);       // Cutoff material property
	

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHeightMapTerrain->Render();
	modelViewMatrixStack.Pop();


	glDisable(GL_CULL_FACE);
	// Render the pyramid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(800.0f, 140.0f, -60.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(glm::vec3(30.f, 30.f, 30.f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPyramid->Render();
	modelViewMatrixStack.Pop();
	glEnable(GL_CULL_FACE);

	// Render the boxes (CLOUD)
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(780.0f, 130.0f, -50.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(glm::vec3(30.f, 5.f, 40.f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBox->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(800.0f, 120.0f, -30.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(glm::vec3(10.f, 5.f, 20.f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBox->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(790.0f, 115.0f, -10.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(glm::vec3(15.f, 5.f, 10.f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBox->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	


	// Render the horse 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(35.0f, 1.0f, 25.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();


	// Render the Car
	modelViewMatrixStack.Push();
		// Car moves left
		if (carTurnLeft == 1)
			modelViewMatrixStack.Translate(glm::vec3(-5.f * N.x + p.x, p.y, -5.f * N.z + p.z));
		// Car moves right
		if (carTurnRight == 1)
			modelViewMatrixStack.Translate(glm::vec3(5.f * N.x + p.x, p.y, 5.f * N.z + p.z));
		// Car stays center
		if (carTurnLeft == 0 && carTurnRight == 0)
			modelViewMatrixStack.Translate(glm::vec3(p.x, p.y, p.z));

		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::atan(T.x, T.z));
		modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(-90.f));
		modelViewMatrixStack.Scale(0.6f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCarMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the Gate
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0, 0.1f, 200));
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-60.f));
	modelViewMatrixStack.Scale(glm::vec3(4, 2, 2));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pGateMesh->Render();
	modelViewMatrixStack.Pop();


	// Render the lamp

	for (int i = 0; i < offsetLeft.size(); i = i + 5)
	{
		if (i == centerPoints.size() - 1)
			T = glm::normalize(centerPoints[0] - centerPoints[i]);
		else
			T = glm::normalize(centerPoints[i + 1] - centerPoints[i]);

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(offsetLeft[i]));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::atan(T.x, T.z));
		modelViewMatrixStack.Scale(1.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pLampMesh->Render();
		modelViewMatrixStack.Pop();

	}

	/*for (int i = 0; i < offsetRight.size(); i = i + 5)
	{
		if (i == centerPoints.size() - 1)
			T = glm::normalize(centerPoints[0] - centerPoints[i]);
		else
			T = glm::normalize(centerPoints[i + 1] - centerPoints[i]);

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(offsetRight[i]));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::atan(T.x, T.z));
		modelViewMatrixStack.Scale(1.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pLampMesh->Render();
		modelViewMatrixStack.Pop();

	}*/

	for (int i = 0; i < offsetLeft.size(); i = i + 2)
	{
		if (i == centerPoints.size() - 1)
			T = glm::normalize(centerPoints[0] - centerPoints[i]);
		else
			T = glm::normalize(centerPoints[i + 1] - centerPoints[i]);

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(offsetLeft[i]));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::atan(T.x, T.z));
		modelViewMatrixStack.Scale(2.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pFenceMesh->Render();
		modelViewMatrixStack.Pop();

	}

	for (int i = 0; i < offsetRight.size(); i = i + 2)
	{
		if (i == centerPoints.size() - 1)
			T = glm::normalize(centerPoints[0] - centerPoints[i]);
		else
			T = glm::normalize(centerPoints[i + 1] - centerPoints[i]);

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(offsetRight[i]));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::atan(T.x, T.z));
		modelViewMatrixStack.Scale(2.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pFenceMesh->Render();
		modelViewMatrixStack.Pop();

	}
	// Render the Centerline
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	//m_pCatmullRom->RenderCentreline(); // Rendering Centreline
	//m_pCatmullRom->RenderOffsetCurves(); // Rendering OffsetCurves
	m_pCatmullRom->CreateTrack();
	m_pCatmullRom->RenderTrack(); // Rendering track
	modelViewMatrixStack.Pop();

	// Switch to the sphere program
	CShaderProgram* pSphereProgram = (*m_pShaderPrograms)[2];
	pSphereProgram->UseProgram();
	// Set light and materials in sphere programme
	pSphereProgram->SetUniform("material2.Ma", glm::vec3(0.0f, 1.0f, 0.0f));
	pSphereProgram->SetUniform("material2.Md", glm::vec3(0.0f, 1.0f, 0.0f));
	pSphereProgram->SetUniform("material2.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("material2.shininess", 50.0f);
	pSphereProgram->SetUniform("light2.La", glm::vec3(0.15f, 0.15f, 0.15f));
	pSphereProgram->SetUniform("light2.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light2.position", viewMatrix * lightPosition1);
	pSphereProgram->SetUniform("t", m_t);
	pSphereProgram->SetUniform("levels", 10);
	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-10.0f, 2.0f, -150.0f));
		modelViewMatrixStack.Scale(2.0f);
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pSphereProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();
		
	

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

	

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt + goFoward);

	m_pAudio->Update(m_pCamera->GetPosition(), m_pCamera->GetStrafeVector(), m_pCamera->GetView(), glm::vec3(p.x, p.y, p.z), m_pCamera->GetUpVector());

	// Increment the distance + speed
	m_currentDistance += m_dt * 0.05f + goFoward;

	//Determine a point on the centerline
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, pNext);

	//TNB 
	T = glm::normalize(glm::vec3(pNext - p));
	N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	// Sphere Shader
	m_t += (float)(0.01f * m_dt);

	// Laps
	m_laps = m_pCatmullRom->CurrentLap(m_currentDistance);
	
	if (cameraSelect == 2)
	{
		//Set camera first person
		m_pCamera->Set(glm::vec3(p.x - 5.f * T.x, p.y + 5.0f, p.z - 5.f * T.z), p + 20.f * T, glm::vec3(0, 1, 0));	
	}
	else if (cameraSelect == 3)
	{
		//Set camera third person
		m_pCamera->Set(glm::vec3(p.x - 20.f * T.x, p.y + 15.0f, p.z - 20.f * T.z), p + 20.f * T, glm::vec3(0, 1, 0));
	}
	else if (cameraSelect == 4)
	{
		//Set camera side view person
		m_pCamera->Set(glm::vec3(p.x - 20.f, p.y + 5, p.z), p + 5.f * T, glm::vec3(0, 1, 0));
	}
	else if (cameraSelect == 5)
	{
		//Set camera top view person
		m_pCamera->Set(glm::vec3(p.x, p.y + 25.f, p.z), p + 5.f * T, glm::vec3(0, 1, 0));
	}
}

void Game::DisplayFrameRate()
{
	
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	//goFoward = 0;
	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;
	m_timePassed += m_elapsedTime;

	
	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// FPS
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
		// Time
		m_pFtFont->Render(20, height - 40, 20, "Time: %.f", m_timePassed / 10000);
		// Laps
		m_pFtFont->Render(20, height - 60, 20, "Lap: %d", m_laps);
		// Speed
		m_pFtFont->Render(20, height - 80, 20, "Speed: %.f", m_dt * goFoward);
		// Positions
		m_pFtFont->Render(20, height - 520, 20, "PosX: %d", m_pCamera->GetPosition().x);
		m_pFtFont->Render(20, height - 540, 20, "PosY: %d", m_pCamera->GetPosition().y);
		m_pFtFont->Render(20, height - 560, 20, "PosZ: %d", m_pCamera->GetPosition().z);
		// Camera controls
		m_pFtFont->Render(620, height - 20, 20, "Controls");
		m_pFtFont->Render(620, height - 40, 20, "FPS Camera: 2");
		m_pFtFont->Render(620, height - 60, 20, "TPS Camera: 3");
		m_pFtFont->Render(620, height - 80, 20, "Side Camera: 4");
		m_pFtFont->Render(620, height - 100, 20, "Top Camera: 5");
		m_pFtFont->Render(620, height - 120, 20, "Free Camera: 6");
		// Car controls
		m_pFtFont->Render(620, height - 140, 20, "Foward: W");
		m_pFtFont->Render(620, height - 160, 20, "Slowdown: S");
		m_pFtFont->Render(620, height - 180, 20, "Left: A");
		m_pFtFont->Render(620, height - 200, 20, "Right: D");
		// Effects
		m_pFtFont->Render(620, height - 220, 20, "Fog: F");
		m_pFtFont->Render(620, height - 240, 20, "Blink: B");
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case '2':
			cameraSelect = 2;
			break;
		case '3':
			cameraSelect = 3;
			break;
		case '4':
			cameraSelect = 4;
			break;
		case '5':
			cameraSelect = 5;
			break;
		case '6':
			cameraSelect = 0;
			break;
		case 'W':
			if (goFoward <= 0.8f)
				goFoward += 0.1f;
			break;
		case 'S':
			if (goFoward >= -0.1f)
				goFoward -= 0.1f;
			break;
		case 'A':
			if (carTurnLeft == 0 || carTurnLeft == -1)
			{
				carTurnRight -= 1;
				carTurnLeft += 1;
			}	
			break;
		case 'D':
			if (carTurnRight == 0 || carTurnRight == -1)
			{
				carTurnLeft -= 1;
				carTurnRight += 1;
			}
			break;
		case 'F':
			if (fogEnable == 0)
				fogEnable += 1;
			else if (fogEnable == 1)
				fogEnable -= 1;
			break;
		case 'B':
			if (blinkEnable == 0)
				blinkEnable += 1;
			else if (blinkEnable == 1)
				blinkEnable -= 1;
			break;
		}
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
