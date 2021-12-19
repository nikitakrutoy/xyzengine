#pragma once

#include "ResourceManager.h"
#include "RenderEngine.h"
#include "InputHandler.h"
#include "EntityManager.h"
#include "ScriptManager.h"
#include "flecs.h"
#include <memory>


class Game 
{
public:
	Game();
	void Run();

	ResourceManager* m_pResourceManager;
	RenderEngine* m_pRenderEngine;
	InputHandler* m_pInputHandler;
	EntityManager* m_pEntityManager;
	ScriptManager* m_pScriptManager;
	flecs::world* m_pECS;
	std::shared_ptr<Ogre::SceneManager> m_pSceneManager;
};

