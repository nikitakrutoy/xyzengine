#include "Editor.h"
#include "Common.h"

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
	while (true)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				exit(0);
				break;
			
			case SDL_KEYDOWN:
				Ogre::LogManager::getSingleton().logMessage("Name" + std::string(SDL_GetKeyName(event.key.keysym.sym)));

			case SDL_MOUSEBUTTONDOWN:
				int x, y;
				SDL_GetMouseState(&x, &y);
				Ogre::LogManager::getSingleton().logMessage(Ogre::String(std::to_string(x) + " " + std::to_string(y) + "\n"));
				break;

			default:
				break;
			}
		}

		m_pRenderEngine->GetRT()->RC_BeginFrame();

		m_pWindowsManager->Update();
		
		m_pRenderEngine->GetRT()->RC_EndFrame();
	}
}
