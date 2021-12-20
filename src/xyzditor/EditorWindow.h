#pragma once
#include "SDL.h"
#include <memory>
#include "imgui.h"
#include "RenderEngine.h"

class EditorWindow
{
public:
	EditorWindow(RenderEngine* renderEngine, std::string name = "SDL Ogre Engine", size_t width = 500, size_t height = 200);

	virtual void Update() {};
	
	void SwapFrames() {SDL_GL_SwapWindow(m_SDL_Window);};
	void SetImguiContext() { ImGui::SetCurrentContext(m_pImGuiContext); }
	SDL_Window* GetWindow() { return m_SDL_Window; };

	void SetSize(int w, int h) {
		width = w;
		height = h;
	}

	virtual ~EditorWindow();

protected:
	int width, height;
	ImGuiContext* m_pImGuiContext;
	SDL_Window* m_SDL_Window;
	SDL_GLContext m_GL_Context;

protected:
	RenderEngine* m_pRenderEngine;
};


class SceneTreeWindow : public EditorWindow
{
public:
	SceneTreeWindow(RenderEngine* renderEngine, std::string name, size_t width = 400, size_t height = 200) : EditorWindow(renderEngine, name, width, height) {
		memset(sceneName, 0, 128); 
		strncpy(sceneName, "default.json", 12);
	};
	~SceneTreeWindow() {};
	void Update() override;
	Ogre::SceneNode* GetSelected() { return m_pSelectedNode;  }

private:
	char sceneName[128];
	void AddObject(Ogre::SceneNode* node);

	void DrawSceneTree(Ogre::SceneNode* node);
	Ogre::SceneNode* m_pSelectedNode = nullptr;
};


class GameObjectEditor : public EditorWindow
{
public:
	GameObjectEditor(RenderEngine* renderEngine, std::string name, size_t width = 400, size_t height = 200) : EditorWindow(renderEngine, name, width, height) {};
	~GameObjectEditor() {};
	void Update() override;
	void SetSelected(Ogre::SceneNode* node);

private:
	Ogre::SceneNode* m_pSelectedNode = nullptr;
	bool visibility = true;
	int lightType = 0;
	char objectName[128];
	std::string meshName;
	std::string scriptName;
	std::string objectType;

	Ogre::Item* item;
	Ogre::Light* light;
};

class DemoWindow : public EditorWindow
{
public:
	DemoWindow(RenderEngine* renderEngine, std::string name, size_t width = 400, size_t height = 200) : EditorWindow(renderEngine, name, width, height) {
	};
	~DemoWindow() {};
	void Update() override;;

};

