#pragma once
#include <OgreVector3.h>
#include "Script.h"
#include "ScriptManager.h"
#include "RenderEngine.h"
struct CameraController {};

struct InputHandlerPtr
{
	class InputHandler* ptr;
};

struct RenderEnginePtr
{
	class RenderEngine* ptr;
};

struct ScriptSystemPtr
{
	ScriptManager* ptr;
};

struct Position : public Ogre::Vector3
{
	using Ogre::Vector3::Vector3;
};

struct ScriptComponent
{
	Script* ptr;
};

struct RenderComponent
{
	Ogre::SceneNode* ptr;
};