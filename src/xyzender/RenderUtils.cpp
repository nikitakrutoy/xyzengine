#include "RenderUtils.h"

void OgreUtils::destroyAllAttachedMovableObjects(Ogre::SceneNode* node)
{
	if (!node) return;

	destroyAttachedItem(node);

	// Recurse to child SceneNodes
	Ogre::SceneNode::NodeVecIterator itChild = node->getChildIterator();

	while (itChild.hasMoreElements())
	{
		Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
		destroyAllAttachedMovableObjects(pChildNode);
	}
}

void OgreUtils::destroyAttachedItem(Ogre::SceneNode* node)
{
	if (!node) return;

	// Destroy all the attached objects
	Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

	while (itObject.hasMoreElements())
		node->getCreator()->destroyMovableObject(itObject.getNext());
}

void OgreUtils::destroySceneNode(Ogre::SceneNode* node)
{
	if (!node) return;
	destroyAllAttachedMovableObjects(node);
	node->removeAndDestroyAllChildren();
	node->getCreator()->destroySceneNode(node);
}

Ogre::Item* OgreUtils::loadMesh(std::string name, Ogre::SceneManager* mgr) {

	Ogre::Item* item;
	try {
		item = mgr->createItem(name,
			Ogre::ResourceGroupManager::
			AUTODETECT_RESOURCE_GROUP_NAME,
			Ogre::SCENE_DYNAMIC);
		item->setName(name);
		return item;
	}
	catch (const std::exception& e) {
		Ogre::v1::MeshPtr v1Mesh;
		Ogre::MeshPtr v2Mesh;
		if (Ogre::MeshManager::getSingleton().getByName(name + "_imported").isNull()) {
			v1Mesh = Ogre::v1::MeshManager::getSingleton().load(
				name, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
				Ogre::v1::HardwareBuffer::HBU_STATIC, Ogre::v1::HardwareBuffer::HBU_STATIC);
			v2Mesh = Ogre::MeshManager::getSingleton().createManual(
				name + "_imported", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			bool halfPosition = true;
			bool halfUVs = true;
			bool useQtangents = true;
			v2Mesh->importV1(v1Mesh.get(), halfPosition, halfUVs, useQtangents);
			v1Mesh->unload();
		}
		item = mgr->createItem(name + "_imported",
			Ogre::ResourceGroupManager::
			AUTODETECT_RESOURCE_GROUP_NAME,
			Ogre::SCENE_DYNAMIC);
		item->setName(name);
		return item;

	}
}
