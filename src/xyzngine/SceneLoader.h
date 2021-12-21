#pragma once

#include "nlohmann/json.hpp"
#include "OgreSceneManager.h"
#include "EntityManager.h"
#include <string>
#include <map>
class SceneLoader
{
public:
	static std::map<Ogre::SceneNode*, std::string>& getNodeScripts() {
		static std::map<Ogre::SceneNode*, std::string> nodeScripts; return nodeScripts;
	};
	static std::map<Ogre::SceneNode*, std::string>& getNodeMaterials() {
		static std::map<Ogre::SceneNode*, std::string> nodeMaterials; return nodeMaterials;
	};
	static void StoreJSON(Ogre::SceneManager* sceneManager, std::string path);
	static void LoadJSON(Ogre::SceneManager* sceneManager, EntityManager* entityManager, std::string path);
};