#pragma once
#include "EditorWindow.h"
#include "RenderEngine.h"
#include <vector>
class WindowsManager
{
public:
	WindowsManager(RenderEngine* renderEngine);
	void Update();
	void ProcessInput();
	std::vector<EditorWindow*> GetWindows() { return { m_pSceneTreeWindow, m_pGameObjectEditor, m_pDemoWindow }; };

private:
	SceneTreeWindow* m_pSceneTreeWindow;
	GameObjectEditor* m_pGameObjectEditor;
	DemoWindow* m_pDemoWindow;
};

