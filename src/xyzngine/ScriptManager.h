#pragma once

#include "Script.h"

class ScriptManager
{
public:
	ScriptManager(InputHandler* pInputHandler, std::string strScriptsRoot);
	~ScriptManager();
	ScriptManager(const ScriptManager&) = delete;
	ScriptManager& operator=(const ScriptManager&) = delete;

	Script* CreateScript(std::string strScriptName, flecs::entity entity);

private:
	std::string m_strScriptsRoot;
	InputHandler* m_pInputHandler;
};

