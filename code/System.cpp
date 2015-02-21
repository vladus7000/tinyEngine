#include "System.hpp"

using namespace std;

void System::Init()
{
	ifstream inputStream("settings/settings.set");
	string line;
	string jsonData;

	if (inputStream)
	{
		while ( getline (inputStream,line) )
		{
			jsonData.append(line+'\n');
		}
	}
	else
	{
		throw Exception("[System] could not open file: Settings.set");
	}

	json::Value sub_obj = json::Deserialize(jsonData);
	json::Value WindowSettings = sub_obj["WindowSettings"];
	json::Value ShadersData = sub_obj["Shader"];

	m_width = WindowSettings["width"];
	m_height = WindowSettings["height"];

	m_name = WindowSettings["title"];
	m_vert = ShadersData["vert"];
	m_frag = ShadersData["frag"];
	m_initScript = sub_obj["InitScript"];

	if (!glfwInit())
		throw Exception("[System] could not open initialize glfw");
	m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);

	glfwMakeContextCurrent(m_window);
	glewExperimental = GL_TRUE;
	glewInit();
}

void System::Deinit()
{
	m_isFullScreen = false;
	glfwTerminate();
}

void System::SetFullScreen()
{
	m_isFullScreen = true;
}

void System::SetWindowedMode()
{
	m_isFullScreen = false;
}

bool System::WindowIsClose()
{
	return glfwWindowShouldClose(m_window);
}

void System::Swap()
{
	glfwSwapBuffers(m_window);
}

void System::PollEvent()
{
	glfwPollEvents();
}