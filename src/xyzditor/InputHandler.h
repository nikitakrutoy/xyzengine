#pragma once
#include <string>
#include <map>
#include <vector>
#include <SDL.h>
#include "EditorWindow.h"



class InputHandler
{
public:
	InputHandler(std::string path);

	void Update(SDL_Window* window, std::vector<EditorWindow*> editorWindows, float deltaTime);

	bool Get(SDL_Keycode key) { return m_keyState.find(key) != m_keyState.end() ? m_keyState[key] : false; }
	bool GetMouse(int key) { return m_mouseState.find(key) != m_mouseState.end() ? m_mouseState[key] : false; }
	float GetMouseDiffX() { return mouseDiffX; }
	float GetMouseDiffY() { return mouseDiffY; }

	bool WantToExit() { return m_exit; }
private:

	std::map<std::string, std::string> m_command2key;
	std::map<std::string, SDL_Keycode> m_key2code;

	std::map<SDL_Keycode, bool> m_keyState;
	std::map<int, bool> m_mouseState;
	bool m_exit = false;

	float mouseDiffX;
	float mouseDiffY;

	int mousePosX;
	int mousePosY;
};

