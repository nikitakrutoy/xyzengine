#pragma once

#include "Ogre.h"
#include "OgreRoot.h"
#include "OgreWindow.h"
#include "Compositor/OgreCompositorWorkspace.h"
#include "Compositor/OgreCompositorManager2.h"

#include "ResourceManager.h"

#include "RenderSystems/GL3Plus/OgreGL3PlusPlugin.h"

#include "RenderThread.h"

#include <memory>

#include "SDL.h"
#include "SDL_opengl.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

#include "SDL_syswm.h"


class RenderEngine
{
	friend class RenderThread;

public:
	RenderEngine(ResourceManager* pResourceManager);
	~RenderEngine();
	RenderEngine(const RenderEngine&) = delete;
	RenderEngine& operator=(const RenderEngine&) = delete;

	void Update();

	bool GetQuit() { return m_bQuit; }
	void SetQuit(bool bQuit) { m_bQuit = bQuit; }


	Ogre::SceneNode* GetRootNode() { return m_pSceneManager->getRootSceneNode(); };
	std::shared_ptr<Ogre::SceneManager> GetSceneManager() { return m_pSceneManager; };
	SDL_Window* GetWindow() { return m_SDL_Window; };

	RenderThread* GetRT() const { return m_pRT.get(); }

	Ogre::Camera* GetMainCamera() { return m_pCamera.get(); }

	bool IsInitialized() { return m_bIsInitialized; }

private:
	bool SetOgreConfig();

	void RT_SDLClenup();
	void RT_Init();
	void RT_InitSDL();
	void RT_SetupDefaultCamera();
	void RT_SetupDefaultCompositor();
	void RT_LoadDefaultResources();
	void RT_LoadDefaultScene();
	void RT_LoadOgreHead();
	void RT_SetupDefaultLight();
	void RT_EndInit();

	SDL_Window* m_SDL_Window;

	std::unique_ptr<Ogre::Root> m_pRoot;
	std::unique_ptr<Ogre::Window> m_pRenderWindow;
	std::shared_ptr<Ogre::SceneManager> m_pSceneManager;
	std::unique_ptr<Ogre::Camera> m_pCamera;
	std::unique_ptr<Ogre::CompositorWorkspace> m_pWorkspace;
	std::unique_ptr<Ogre::GL3PlusPlugin> m_pGL3PlusPlugin;

	ResourceManager* m_pResourceManager;

	std::unique_ptr<RenderThread> m_pRT;

	SDL_GLContext m_GL_Context;

	bool m_bIsInitialized;
	bool m_bQuit;
};

