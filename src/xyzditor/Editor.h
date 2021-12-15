#pragma once

#include "ResourceManager.h"
#include "RenderEngine.h"
#include "WindowsManager.h"
#include <memory>

class Editor
{
public:
	Editor();
	~Editor();
	void Run();

	ResourceManager* m_pResourceManager;
	RenderEngine* m_pRenderEngine;
	WindowsManager* m_pWindowsManager;
	std::shared_ptr<Ogre::SceneManager> m_pSceneManager;

};

