#include "ScriptManager.h"


ScriptManager::ScriptManager(InputHandler* pInputHandler, std::string strScriptsRoot) :
	m_pInputHandler(pInputHandler),
	m_strScriptsRoot(strScriptsRoot)
{

}

ScriptManager::~ScriptManager()
{

}

Script* ScriptManager::CreateScript(std::string strScriptName, flecs::entity entity)
{
	std::string strScriptPath = m_strScriptsRoot + strScriptName;

	Script* pScript = new Script(strScriptPath, m_pInputHandler, entity);

	return pScript;
}
