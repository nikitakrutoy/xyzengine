#include "EntityManager.h"
#include "ecsComponents.h"

EntityManager::EntityManager(RenderEngine* pRenderEngine, ScriptManager* pScriptManager, flecs::world* ecs) :
	m_pRenderEngine(pRenderEngine),
	m_pEcs(ecs),
	m_pScriptManager(pScriptManager)
{

}


void EntityManager::CreateEntity(Ogre::SceneNode* sceneNode, std::string strScriptName)
{
	flecs::entity newEntity = m_pEcs->entity();

	Script* pScript = m_pScriptManager->CreateScript(strScriptName, newEntity);
	Transform transform = Transform{
		sceneNode->getPosition(),
		sceneNode->getScale(),
		sceneNode->getOrientation()
	};
	auto cam = sceneNode->getCreator()->getCameras().at(0);
	Transform camTransform = Transform{
		cam->getPosition(),
		Ogre::Vector3::ZERO,
		cam->getOrientation()
	};
	pScript->InitTransform(transform, "Transform");
	pScript->InitTransform(camTransform, "CameraTransform");
	pScript->Initialize();
	pScript->GetComponents(newEntity);


	newEntity.set(ScriptComponent{ pScript });
	newEntity.set(RenderComponent{ sceneNode });
	newEntity.set(Transform(transform));
}