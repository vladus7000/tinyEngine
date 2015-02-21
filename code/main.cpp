#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ShaderProgram.hpp"
#include "Exception.hpp"
#include <fstream>
#include "Model.hpp"
#include "Material.hpp"
#include "Stage.hpp"
#include <memory>
#include "json/json.h"
#include <map>
#include <stdlib.h>

//#define LUA_BUILD_AS_DLL
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <LuaBridge.h>


using namespace luabridge;

using namespace std;
using namespace glm;
unsigned int counter = 0;

void printMessage(const std::string& s) {
    std::cout << s << std::endl;
}

class LuaScript;
void _terminateScript(unsigned int  self);
map<unsigned int, LuaScript*> scriptsMap;
glm::mat4 g_proj;
int width;
int height;

bool projChanged = false;
bool newModel = false;
vector<Model *> g_model;
void test()
{
	cout << "TEST\n";
}

void setProjection(float angle, float near, float far)
{
	projChanged = true;

	g_proj = glm::mat4(1.0f);
	g_proj = glm::perspective(angle * 3.141592654f / 180.0f/*3.141592654f / 3.0f*/, (float)width / (float)height, near, far);
}

class LuaModel
{
public:
	LuaModel(const std::string& path)
		:model(path)
	{
		m_translate = glm::mat4(1.0f);
		m_rotate = glm::mat4(1.0f);
		m_scale = glm::mat4(1.0f);
		g_model.push_back(&model);
		newModel = true;
	}

	void setRotate(float angle, float x, float y, float z)
	{
		m_rotate = glm::rotate(glm::mat4(1.0), angle * 3.141592654f / 180.0f, glm::vec3(x, y, z));
		glm::mat4 world = m_translate * m_scale * m_rotate;
		model.SetWorldMatrix(world);
	}
	void setScale(float x, float y, float z)
	{
		m_scale = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z));
		glm::mat4 world = m_translate * m_scale * m_rotate;
		model.SetWorldMatrix(world);
	}
	void setTranslate(float x, float y, float z)
	{
		m_translate = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
		glm::mat4 world = m_translate * m_scale * m_rotate;
		model.SetWorldMatrix(world);
	}
	~LuaModel()
	{
	}
private:
	glm::mat4 m_translate;
	glm::mat4 m_rotate;
	glm::mat4 m_scale;
	Model model;
};

class LuaScript
{
public:
	LuaScript(const char* path)
		: m_L(0)
		, m_isTerminated(false)
		, m_mainLoopFunction(0)
		, m_initFunction(0)
		, m_deinitFunction(0)
	{
		m_L = luaL_newstate();

		luaL_openlibs(m_L);
		lua_pcall(m_L, 0, 0, 0);
			getGlobalNamespace(m_L).\
				beginNamespace("engine").\
					addFunction("terminateScript", _terminateScript).\
					addFunction("setProjection", setProjection).\
				endNamespace();

			getGlobalNamespace(m_L).\
				beginNamespace("engine").\
					beginNamespace("io").\
						addFunction("printMessage", printMessage).\
					endNamespace().\
				endNamespace();

			getGlobalNamespace(m_L).\
				addFunction("test", test);

			getGlobalNamespace(m_L)\
					.beginNamespace("engine")\
					.beginClass<LuaModel>("Model")\
						.addConstructor <void (*) (const std::string& name)> ()\
						.addFunction("setRotate", &LuaModel::setRotate)\
						.addFunction("setScale", &LuaModel::setScale)\
						.addFunction("setTranslate", &LuaModel::setTranslate)\
					.endClass().\
					endNamespace();
		if (luaL_dofile(m_L, path))
		{
			terminateScript();
			return;
		}

		m_mainLoopFunction = new LuaRef(getGlobal(m_L, "MainLoop"));
		m_initFunction = new LuaRef(getGlobal(m_L, "init"));
		m_deinitFunction = new LuaRef(getGlobal(m_L, "deinit"));

		if (m_initFunction->isFunction())
		{
			scriptsMap[counter] = this;
			(*m_initFunction)(counter);
			counter++;
		}
	}

	lua_State *getState() {return m_L;}

	void terminateScript()
	{
		if (m_isTerminated) return;
		m_isTerminated = true;
		if (m_deinitFunction != 0 && m_deinitFunction->isFunction())
			{
				(*m_deinitFunction)();
			}
	}

	bool isTerminated() {return m_isTerminated;}

	void mainLoop(float delta)
	{
		if (m_isTerminated) return;
		if (m_mainLoopFunction->isFunction())
		{
			(*m_mainLoopFunction)(delta);
		}
	}

	~LuaScript()
	{
		terminateScript();
		if (m_mainLoopFunction)
			delete m_mainLoopFunction;

		if (m_initFunction)
				delete m_initFunction;

		if (m_deinitFunction)
				delete m_deinitFunction;
		if (m_L)
			lua_close(m_L);
	}
private:
	bool m_isTerminated;
	lua_State* m_L;
	LuaRef *m_mainLoopFunction;
	LuaRef *m_initFunction;
	LuaRef *m_deinitFunction;
};

void _terminateScript(unsigned int self)
{
	map<unsigned int, LuaScript*>::iterator it= scriptsMap.find(self);
	if (it != scriptsMap.end())
	{
		it->second->terminateScript();
	}
}

int main (int argc,const char **argv)
{
	GLFWwindow* window = NULL;
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
		throw Exception("[Main] could not open file: Settings.set");
	}

	json::Value sub_obj = json::Deserialize(jsonData);
	json::Value WindowSettings = sub_obj["WindowSettings"];
	json::Value ShadersData = sub_obj["Shader"];

	width = WindowSettings["width"];
	height = WindowSettings["height"];

	string windowName = WindowSettings["title"];
	string vert = ShadersData["vert"];
	string frag = ShadersData["frag"];
	string initScript = sub_obj["InitScript"];

	if (!glfwInit())
		return -1;
	window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	LuaScript firstScript(initScript.c_str());
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	float aspect = (float)width / (float)height;
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(45.0f * 3.141592654f / 180.0f, 800.0f / 600.0f, 0.3f, 100.f);
	ShaderProgram simpleProgram;
	try
	{
		simpleProgram.AttachVertexShader(vert);
		simpleProgram.AttachPixelShader(frag);
		simpleProgram.CreateShaderProgram();
	}
	catch (Exception &e)
	{
		std::cout << e.m_errorMsg << std::endl;
		aiDetachAllLogStreams();
		glfwTerminate();
		return 0;
	}
	Stage simpleStage;
	simpleStage.SetWindowSize(width, height);
	simpleStage.SetShader(simpleProgram);
	simpleStage.SetProjectionMatrixConstant(proj);
	aiDetachAllLogStreams();

	float angle = 0.0;
	static float delta = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		if (projChanged)
		{
			simpleStage.SetProjectionMatrixConstant(g_proj);
			projChanged = false;
		}
		if (newModel)
		{
			for (unsigned int i = 0; i < g_model.size(); i++)
				simpleStage.AddModelToStage(g_model[i]);
			g_model.clear();
			newModel = false;
		}
		delta += 0.001;
		simpleStage.Render();
		glfwSwapBuffers(window);
		glfwPollEvents();
		firstScript.mainLoop(delta);

	}
	glfwTerminate();
	return 0;
}
