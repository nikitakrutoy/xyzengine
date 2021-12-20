#pragma once
#include "OgreItem.h"
#include "OgreSceneManager.h"
#include "OgreMesh2.h"
#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"

struct Transform {
	Ogre::Vector3 Position;
	Ogre::Vector3 Scale;
	Ogre::Quaternion Orientation;
};


namespace OgreUtils
{
	void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);

	void destroyAttachedItem(Ogre::SceneNode* node);

	void destroySceneNode(Ogre::SceneNode* node);

	Ogre::Item* loadMesh(std::string name, Ogre::SceneManager* mgr);
}

