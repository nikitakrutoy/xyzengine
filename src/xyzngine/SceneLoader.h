#pragma once

#include "nlohmann/json.hpp"
#include "OgreSceneManager.h"
#include "EntityManager.h"
#include <string>
class SceneLoader
{
public:
	static void StoreJSON(Ogre::SceneManager* sceneManager, std::string path);
	static void LoadJSON(Ogre::SceneManager* sceneManager, std::string path);
	static void LoadJSON2ECS(Ogre::SceneManager* sceneManager, EntityManager* entityManager, std::string path);
};