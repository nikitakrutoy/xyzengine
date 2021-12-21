#pragma once

#include <string>

#include "InputHandler.h"

#include "LuaBridge.h"
#include "flecs.h"
#include "Ogre.h"
#include "RenderUtils.h"

class Script
{
public:
	Script(std::string strScriptPath, InputHandler* pInputHandler, flecs::entity& ent);
	~Script();

	void Update(float dt);
	void Initialize();
	void ReloadScript();
	void GetComponents(flecs::entity& ent);

	void InitTransform(Transform transform, std::string name = "Transform");
	void SetTransform(Transform transform, std::string name = "Transform");
	Transform GetTransform(std::string name = "Transform");


	bool GetIsStatic() const;

private:
	std::string m_strScriptPath;

	bool m_isTransformIntialized = false;

	InputHandler* m_pInputHandler;

	lua_State* m_script;

	void AddDependencies(lua_State* L);

	const char* m_EntityFieldName = "Entity";
	const char* m_PropertiesFieldName = "Properties";
	const char* m_ControllableFieldName = "Controllable";
	const char* m_NameFieldName = "Name";
	const char* m_ParametersFieldName = "Parameters";
	const char* m_StaticsFieldName = "IsStatic";

	const char* m_OnInitFunctionName = "OnInit";
	const char* m_OnUpdateFunctionName = "OnUpdate";
};
