#include "SceneLoader.h"
#include "OgreItem.h"
#include "OgreMesh2.h"
#include "RenderUtils.h"
#include "OgreCamera.h"
#include <fstream>

using json = nlohmann::json;

void SceneLoader::StoreJSON(Ogre::SceneManager* sceneManager, std::string path)
{
	auto d = json::array();

	auto childs = sceneManager->getRootSceneNode()->getChildIterator();

	while (childs.hasMoreElements()) {
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*> (childs.getNext());
		auto childObj = json::object();
		childObj["name"] = child->getName();
		auto position = child->getPosition();
		auto scale = child->getScale();

		childObj["position"] = json::array({ position.x, position.y, position.z });
		childObj["scale"] = json::array({ scale.x, scale.y, scale.z });
		auto scriptName = child->getUserObjectBindings().getUserAny("scriptName");
		if (!scriptName.isEmpty()) 
			childObj["scriptName"] = Ogre::any_cast<std::string>(scriptName); 
		Ogre::SceneNode::ObjectIterator iter = child->getAttachedObjectIterator();
		while (iter.hasMoreElements()) {
			Ogre::MovableObject* movObj = iter.getNext();
			childObj["type"] = movObj->getMovableType();

			if (childObj["type"] == "Item") {
				Ogre::Item* item = dynamic_cast<Ogre::Item*>(movObj);
				childObj["meshName"] = item->getMesh().get()->getName();
			}
			if (childObj["type"] == "Light") {
				Ogre::Light* light = dynamic_cast<Ogre::Light*>(movObj);
				childObj["lightType"] = light->getType();
				auto dir = light->getDirection();
				childObj["direction"] = json::array({ dir.x, dir.y, dir.z });
			}
		}
		d.push_back(childObj);
	}
	std::ofstream o(path);
	o << std::setw(4) << d << std::endl;
}

void SceneLoader::LoadJSON(Ogre::SceneManager* sceneManager, std::string path)
{
	//OgreUtils::destroyAllAttachedMovableObjects(sceneManager->getRootSceneNode());
	//sceneManager->getRootSceneNode()->removeAndDestroyAllChildren();

	sceneManager->clearScene(true);
	auto cameraIter = sceneManager->getCameraIterator();
	while (cameraIter.hasMoreElements()) {
		auto m_pCamera = cameraIter.getNext();
		m_pCamera->setPosition(Ogre::Vector3(150, 0, 0));
		m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
		m_pCamera->setNearClipDistance(0.2f);
		m_pCamera->setFarClipDistance(1000.0f);
		m_pCamera->setAutoAspectRatio(true);
		m_pCamera->setFixedYawAxis(true);
	}

	std::ifstream i(path);
	json j;
	i >> j;
	for (auto obj : j) {

		Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
			createChildSceneNode(Ogre::SCENE_DYNAMIC);
		sceneNode->setName(obj["name"]);
		auto position = obj["position"];
		auto scale = obj["scale"];
		float x = position[0];
		sceneNode->setPosition(position[0], position[1], position[2]);
		sceneNode->setScale(scale[0], scale[1], scale[2]);

		if (obj["type"] == "Item") {
			 Ogre::Item* item = OgreUtils::loadMesh(obj["meshName"], sceneManager);

			item->setName(obj["meshName"]);
			sceneNode->attachObject(item);
		}

		if (obj["type"] == "Light") {
			Ogre::Light* light = sceneManager->createLight();
			sceneNode->attachObject(light);
			light->setPowerScale(Ogre::Math::PI); 
			light->setType(obj["lightType"]);
			auto dir = obj["direction"];
			light->setDirection(Ogre::Vector3(dir[0], dir[1], dir[2]));
		}
	}
}

void SceneLoader::LoadJSON2ECS(Ogre::SceneManager* sceneManager, EntityManager* entityManager, std::string path)
{
	sceneManager->clearScene(true);
	auto cameraIter = sceneManager->getCameraIterator();
	while (cameraIter.hasMoreElements()) {
		auto m_pCamera = cameraIter.getNext();
		m_pCamera->setPosition(Ogre::Vector3(150, 0, 0));
		m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
		m_pCamera->setNearClipDistance(0.2f);
		m_pCamera->setFarClipDistance(1000.0f);
		m_pCamera->setAutoAspectRatio(true);
		m_pCamera->setFixedYawAxis(true);
	}

	std::ifstream i(path);
	json j;
	i >> j;
	for (auto obj : j) {
		Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
			createChildSceneNode(Ogre::SCENE_DYNAMIC);
		sceneNode->setName(obj["name"]);
		auto position = obj["position"];
		auto scale = obj["scale"];
		float x = position[0];
		sceneNode->setPosition(position[0], position[1], position[2]);
		sceneNode->setScale(scale[0], scale[1], scale[2]);

		if (obj["type"] == "Item") {
			Ogre::Item* item = OgreUtils::loadMesh(obj["meshName"], sceneManager);

			item->setName(obj["meshName"]);
			sceneNode->attachObject(item);
		}

		if (obj["type"] == "Light") {
			Ogre::Light* light = sceneManager->createLight();
			sceneNode->attachObject(light);
			light->setPowerScale(Ogre::Math::PI);
			light->setType(obj["lightType"]);
			auto dir = obj["direction"];
			light->setDirection(Ogre::Vector3(dir[0], dir[1], dir[2]));
		}
		if (obj.find("scriptName") != obj.end())
			entityManager->CreateEntity(sceneNode, obj["scriptName"]);
	}
}
