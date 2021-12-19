#include "InputHandler.h"
#include "Scancodes.h"
#include <algorithm>

#include "Ogre.h"
#include "OgreConfigFile.h"


InputHandler::InputHandler(std::string path)
{
	auto m_strMapFilePath = path + "actionmap.ini";
	std::replace(m_strMapFilePath.begin(), m_strMapFilePath.end(), '\\', '/');

	Ogre::ConfigFile cf;
	cf.load(m_strMapFilePath);

	Ogre::ConfigFile::SectionIterator secIter = cf.getSectionIterator();

	for (int i = 0; i < SCANCODES.size(); i++) {
		if (!SCANCODES[i].empty()) {
			m_key2code[SCANCODES[i]] = SDL_Scancode(i);
		}
	}

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

void InputHandler::Update(SDL_Window* window, float deltaTime)
{
	SDL_Event ev;
	SDL_Scancode key;
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	mouseDiffX = 0.f;
	mouseDiffY = 0.f;
	while (SDL_PollEvent(&ev))
	{
		
		if (ev.window.windowID != SDL_GetWindowID(window)) continue;
		if (ev.window.event == SDL_WINDOWEVENT_CLOSE) m_exit = true;

		switch (ev.type)
		{
		case SDL_KEYDOWN:
			key = ev.key.keysym.scancode;
			m_keyState[key] = true;
			break;

		case SDL_KEYUP:
			key = ev.key.keysym.scancode;
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

bool InputHandler::GetCommand(std::string command) const
{
	std::string key;
	SDL_Scancode code;
	if (m_command2key.find(command) != m_command2key.end())
		key = m_command2key.at(command);
	else
		return false;
	if (m_key2code.find(key) != m_key2code.end())
		code = m_key2code.at(key);
	else
		return false;
	return m_keyState.find(code) != m_keyState.end() ? m_keyState.at(code) : false;
}
