#pragma once
#include "SDL.h"
#include <memory>
#include "imgui.h"
#include "RenderEngine.h"

class EditorWindow
{
public:
	EditorWindow(RenderEngine* renderEngine, size_t width = 200, size_t height = 500);

	virtual void Update() {};
	
	void SwapFrames() {SDL_GL_SwapWindow(m_SDL_Window);};

	virtual ~EditorWindow();

private:

protected:
	ImGuiContext* m_pImGuiContext;
	SDL_Window* m_SDL_Window;
	SDL_GLContext m_GL_Context;
	SDL_Renderer* m_pSDLRenderer;

protected:
	RenderEngine* m_pRenderEngine;
};


class SceneTreeWindow : public EditorWindow
{
public:
	SceneTreeWindow(RenderEngine* renderEngine, size_t width = 200, size_t height = 500) : EditorWindow(renderEngine, width, height) {};
	~SceneTreeWindow() {};
	void Update() override;
	Ogre::SceneNode* GetSelected() { return m_pSelectedNode;  }

private:

	void DrawSceneTree(Ogre::SceneNode* node);
	Ogre::SceneNode* m_pSelectedNode = nullptr;
};


class GameObjectEditor : public EditorWindow
{
public:
	GameObjectEditor(RenderEngine* renderEngine, size_t width = 200, size_t height = 500) : EditorWindow(renderEngine, width, height) {};
	~GameObjectEditor() {};
	void Update() override;
	void SetSelected(Ogre::SceneNode* node);

private:
	Ogre::SceneNode* m_pSelectedNode = nullptr;
	char objectName[128];
	std::string meshName;
	std::string objectType;
};


