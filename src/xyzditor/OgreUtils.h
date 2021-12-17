#pragma once
#include "OgreItem.h"
#include "OgreSceneManager.h"
#include "OgreMesh2.h"
#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"


namespace OgreUtils
{
	void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);

	void destroyAttachedItem(Ogre::SceneNode* node);

	void destroySceneNode(Ogre::SceneNode* node);

	Ogre::Item* loadMesh(std::string name, Ogre::SceneManager* mgr);
}

