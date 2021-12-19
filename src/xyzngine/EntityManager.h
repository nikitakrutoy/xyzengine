#pragma once

#include <string>
#include "flecs.h"
#include "Ogre.h"
#include "RenderEngine.h"
#include "ScriptManager.h"


class EntityManager
{
public:
	EntityManager(RenderEngine* pRenderEngine, ScriptManager* pScriptSystem, flecs::world* ecs);
	~EntityManager();
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	void CreateEntity(Ogre::SceneNode* sceneNode, std::string strScriptName);


private:
	ScriptManager* m_pScriptManager;
	RenderEngine* m_pRenderEngine;
	flecs::world* m_pEcs;
};