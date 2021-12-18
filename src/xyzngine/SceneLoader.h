#pragma once

#include "nlohmann/json.hpp"
#include "OgreSceneManager.h"
#include <string>
class SceneLoader
{
public:
	static void StoreXML(Ogre::SceneManager* sceneManager, std::string path);
	static void LoadXML(Ogre::SceneManager* sceneManager, std::string path);

};