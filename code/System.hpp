#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "json/json.h"
#include "Exception.hpp"

class System
{
public:
	System() : m_isFullScreen(false) {}
	~System() {};
	void Init();
	void Deinit();
	unsigned int GetWidth() {return m_width;}
	unsigned int GetHeight() {return m_height;}
	std::string GetName() {return m_name;}
	std::string GetInitScriptName() {return m_initScript;}
	std::string GetVert() {return m_vert;}
	std::string GetFrag() {return m_frag;}
	void SetFullScreen();
	void SetWindowedMode();
	bool IsFullScreen() {return m_isFullScreen;}
	bool WindowIsClose();
	void Swap();
	void PollEvent();
private:
	int m_width;
	int m_height;
	bool m_isFullScreen;
	std::string m_name;
	GLFWwindow* m_window;
	std::string m_initScript;
	std::string m_vert;
	std::string m_frag;
};

#endif //_SYSTEM_HPP_