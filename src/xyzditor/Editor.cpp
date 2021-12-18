#include "Editor.h"
#include "Common.h"
#include "SceneLoader.h"

#include "Ogre.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

Editor::Editor()
{
	// GL 3.0 + GLSL 130
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_Init(SDL_INIT_VIDEO);

	m_pResourceManager = new ResourceManager("..\\..\\Media\\");
	m_pRenderEngine = new RenderEngine(m_pResourceManager);
	m_pRenderEngine->GetRT()->RC_EndFrame();

	while (!m_pRenderEngine->IsInitialized()) {};

	m_pSceneManager = m_pRenderEngine->GetSceneManager();
	m_pWindowsManager = new WindowsManager(m_pRenderEngine);
	m_pInputHandler = new InputHandler("..\\..\\Media\\");
}

Editor::~Editor()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_Quit();
}

void Editor::Run()
{
	SDL_Event ev;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;
	double speed = 1000;
	
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(m_pRenderEngine->GetWindow(), &info);
	MessageBox(info.info.win.window, L"Hello", L"Caption", MB_OKCANCEL);

	while (true)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()) * speed;
		

		m_pRenderEngine->GetRT()->RC_BeginFrame();
		m_pWindowsManager->ProcessInput();
		m_pInputHandler->Update(m_pRenderEngine->GetWindow(), deltaTime);
		m_pWindowsManager->Update();

		m_pRenderEngine->GetMainCamera();
		if (m_pInputHandler->Get(SDLK_w))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime] {
				c->setPosition(c->getDirection() * deltaTime + c->getPosition());
			});

		if (m_pInputHandler->Get(SDLK_s))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime]{
				c->setPosition(c->getPosition()  - c->getDirection() * deltaTime);
			});

		if (m_pInputHandler->Get(SDLK_d))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime]{
				c->setPosition(c->getPosition() + c->getRight() * deltaTime);
			});

		if (m_pInputHandler->Get(SDLK_a))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime]{
				c->setPosition(c->getPosition() - c->getRight() * deltaTime);
			});

		if (m_pInputHandler->Get(SDLK_e))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime]{
				c->setPosition(c->getPosition() + c->getUp() * deltaTime);
			});

		if (m_pInputHandler->Get(SDLK_q))
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), deltaTime]{
				c->setPosition(c->getPosition() - c->getUp() * deltaTime);
			});

		if (m_pInputHandler->GetMouse(SDL_BUTTON_RIGHT)) {
			float diff = -m_pInputHandler->GetMouseDiffY() * deltaTime * 1;
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), diff]{
				c->rotate(Ogre::Quaternion(Ogre::Radian(diff), c->getRight()));
			});
		}

		if (m_pInputHandler->GetMouse(SDL_BUTTON_RIGHT)) {
			float diff = -m_pInputHandler->GetMouseDiffX() * deltaTime * 1;
			m_pRenderEngine->GetRT()->RC_LambdaAction([c = m_pRenderEngine->GetMainCamera(), diff]{
				c->yaw(Ogre::Radian(diff));
			});
		}

		if (m_pInputHandler->WantToExit()) exit(0);
		
		m_pRenderEngine->GetRT()->RC_EndFrame();
	}
}
