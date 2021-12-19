#include "Game.h"
#include "RenderEngine.h"
#include "SDL.h"

#include "SceneLoader.h"
#include "EntityManager.h"
#include "ScriptManager.h"

#include "ecsSceneObject.h"
#include "ecsScript.h"

Game::Game()
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
	m_pInputHandler = new InputHandler("..\\..\\Media\\");
	m_pRenderEngine = new RenderEngine(m_pResourceManager);
	m_pECS = new flecs::world();
	m_pScriptManager = new ScriptManager(m_pInputHandler, "..\\..\\Media\\scripts\\");
	m_pEntityManager = new EntityManager(m_pRenderEngine, m_pScriptManager, m_pECS);
	m_pRenderEngine->GetRT()->RC_LambdaAction([=] {
		m_pRenderEngine->RT_SetupDefaultCamera();
		m_pRenderEngine->RT_SetupDefaultCompositor();
		m_pRenderEngine->RT_LoadDefaultResources();
		auto scenesPath = Ogre::ResourceGroupManager::getSingleton().listResourceLocations("Scenes")->at(0);
		scenesPath += "/default.json";
		SceneLoader::LoadJSON2ECS(m_pRenderEngine->GetSceneManager().get(), m_pEntityManager, scenesPath);
		m_pRenderEngine->RT_EndInit();
		});
	m_pRenderEngine->GetRT()->RC_EndFrame();

	while (!m_pRenderEngine->IsInitialized()) {};
	m_pSceneManager = m_pRenderEngine->GetSceneManager();

	register_ecs_script_systems(m_pECS);
	register_ecs_control_systems(m_pECS);
	register_ecs_render_systems(m_pECS);

	auto inputHandler = m_pECS->entity("inputHandler")
		.set(InputHandlerPtr{ m_pInputHandler });
	auto renderEngine = m_pECS->entity("renderEngine")
		.set(RenderEnginePtr{ m_pRenderEngine });
}


void Game::Run()
{
	SDL_Event ev;
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;
	double speed = 1000;

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(m_pRenderEngine->GetWindow(), &info);
	//MessageBox(info.info.win.window, L"Hello", L"Caption", MB_OKCANCEL);
	while (true)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()) * speed;
		m_pRenderEngine->GetRT()->RC_BeginFrame();
		m_pInputHandler->Update(m_pRenderEngine->GetWindow(), deltaTime);
		if (m_pInputHandler->WantToExit()) exit(0);
		m_pECS->progress();
		m_pRenderEngine->GetRT()->RC_EndFrame();
	}
}