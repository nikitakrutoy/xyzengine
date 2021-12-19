#pragma once

#include <string>

#include "InputHandler.h"

#include "LuaBridge.h"
#include "flecs.h"
#include "Ogre.h"

class Script
{
public:
	Script(std::string strScriptPath, InputHandler* pInputHandler, flecs::entity& ent);
	~Script();

	void Update(float dt);
	void ReloadScript();

	Ogre::Vector3 GetPosition() const;
	void SetPosition(Ogre::Vector3 position);

	Ogre::Vector3 GetCameraPosition() const;
	Ogre::Quaternion GetOrientation() const;
	std::string GetMeshName() const;
	bool GetIsStatic() const;

private:
	std::string m_strScriptPath;

	InputHandler* m_pInputHandler;

	lua_State* m_script;

	void AddDependencies(lua_State* L);

	const char* m_EntityFieldName = "Entity";
	const char* m_PropertiesFieldName = "Properties";
	const char* m_ControllableFieldName = "Controllable";
	const char* m_CameraFieldName = "Camera";
	const char* m_MeshNameFieldName = "MeshName";
	const char* m_NameFieldName = "Name";
	const char* m_ParametersFieldName = "Parameters";
	const char* m_StaticsFieldName = "IsStatic";

	const char* m_GetPositionFunctionName = "GetPosition";
	const char* m_SetPositionFunctionName = "SetPosition";
	const char* m_GetCameraPositionFunctionName = "GetCameraPosition";
	const char* m_GetCameraOffsetFunctionName = "GetCameraOffset";
	const char* m_SetCameraOffsetFunctionName = "SetCameraOffset";
	const char* m_GetOrientationFunctionName = "GetOrientation";
	const char* m_OnInitFunctionName = "OnInit";
	const char* m_OnUpdateFunctionName = "OnUpdate";
};
