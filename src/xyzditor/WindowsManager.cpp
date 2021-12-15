#include "WindowsManager.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

WindowsManager::WindowsManager(RenderEngine* renderEngine)
{
	m_pSceneTreeWindow = new SceneTreeWindow(renderEngine);
	m_pGameObjectEditor = new GameObjectEditor(renderEngine);
}

void WindowsManager::Update()
{
	m_pSceneTreeWindow->Update();
	m_pGameObjectEditor->SetSelected(m_pSceneTreeWindow->GetSelected());
	m_pGameObjectEditor->Update();
}
