#include "InputHandler.h"
#include <algorithm>
#include <OgreConfigFile.h>

#include "imgui_impl_sdl.h"
#include "Ogre.h"


InputHandler::InputHandler(std::string path)
{
	auto m_strMapFilePath = path + "actionmap.ini";
	std::replace(m_strMapFilePath.begin(), m_strMapFilePath.end(), '\\', '/');

	Ogre::ConfigFile cf;
	cf.load(m_strMapFilePath);

	Ogre::ConfigFile::SectionIterator secIter = cf.getSectionIterator();

	Ogre::String strInputType, strCommand, strSymbol;
	while (secIter.hasMoreElements())
	{
		strInputType = secIter.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = secIter.getNext();

		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			strCommand = i->first;
			strSymbol = i->second;

			m_command2key[strCommand] = strSymbol;
		}
	}


}

void InputHandler::Update(SDL_Window* window, std::vector<EditorWindow*> editorWindows, float deltaTime)
{
	SDL_Event ev;
	SDL_Keycode key;
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	mouseDiffX = 0.f;
	mouseDiffY = 0.f;
	while (SDL_PollEvent(&ev))
	{
		for (auto w : editorWindows) {
			if (ev.window.windowID == SDL_GetWindowID(w->GetWindow())) {
				w->SetImguiContext();
				ImGui_ImplSDL2_ProcessEvent(&ev);
				break;
			}
		}
		
		if (ev.window.windowID != SDL_GetWindowID(window)) continue;
		if (ev.window.event == SDL_WINDOWEVENT_CLOSE) m_exit = true;

		switch (ev.type)
		{
		case SDL_KEYDOWN:
			key = ev.key.keysym.sym;
			m_keyState[key] = true;
			break;

		case SDL_KEYUP:
			key = ev.key.keysym.sym;
			m_keyState[key] = false;
			break;

		//int x, y;
		//SDL_GetMouseState(&x, &y);
		case SDL_MOUSEMOTION:
			mouseDiffX = float(ev.motion.xrel) / width;
			mouseDiffY = float(ev.motion.yrel) / height;
			mousePosY = ev.motion.x;
			mousePosX = ev.motion.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			m_mouseState[ev.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			m_mouseState[ev.button.button] = false;
			break;
		default:
			break;
		}
	}
}
