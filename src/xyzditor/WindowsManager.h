#pragma once
#include "EditorWindow.h"
#include "RenderEngine.h"
class WindowsManager
{
public:
	WindowsManager(RenderEngine* renderEngine);
	void Update();

private:
	SceneTreeWindow* m_pSceneTreeWindow;
	GameObjectEditor* m_pGameObjectEditor;
};

