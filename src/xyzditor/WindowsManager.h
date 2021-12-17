#pragma once
#include "EditorWindow.h"
#include "RenderEngine.h"
class WindowsManager
{
public:
	WindowsManager(RenderEngine* renderEngine);
	void Update();
	std::vector<EditorWindow*> GetWindows() { return { m_pSceneTreeWindow, m_pGameObjectEditor }; };

private:
	SceneTreeWindow* m_pSceneTreeWindow;
	GameObjectEditor* m_pGameObjectEditor;
};

