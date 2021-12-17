#include "WindowsManager.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

WindowsManager::WindowsManager(RenderEngine* renderEngine)
{
	m_pGameObjectEditor = new GameObjectEditor(renderEngine, "Property Editor");
	m_pSceneTreeWindow = new SceneTreeWindow(renderEngine, "Scene Tree");
}

void WindowsManager::Update()
{
	m_pSceneTreeWindow->Update();
	m_pGameObjectEditor->SetSelected(m_pSceneTreeWindow->GetSelected());
	m_pGameObjectEditor->Update();
}
