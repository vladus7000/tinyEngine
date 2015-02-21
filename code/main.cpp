#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.hpp"
#include "Exception.hpp"
#include "Model.hpp"
#include "Stage.hpp"
#include <memory>
#include <map>
#include <stdlib.h>
#include "FileAbstraction.hpp"
#include "System.hpp"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <LuaBridge.h>

using namespace luabridge;

System g_system;

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

bool projChanged = false;
bool newModel = false;
vector<Model *> g_model;

void setProjection(float a_angle, float a_near, float a_far)
{
	projChanged = true;

	g_proj = glm::mat4(1.0f);
	g_proj = glm::perspective(a_angle * 3.141592654f / 180.0f, (float)g_system.GetWidth() / (float)g_system.GetHeight(), a_near, a_far);
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

int main (int argc, const char **argv)
{
	g_system.Init();
	LuaScript firstScript(g_system.GetInitScriptName().c_str());
	ShaderProgram simpleProgram;
	try
	{
		simpleProgram.AttachVertexShader(g_system.GetVert());
		simpleProgram.AttachPixelShader(g_system.GetFrag());
		simpleProgram.CreateShaderProgram();
	}
	catch (Exception &e)
	{
		std::cout << e.m_errorMsg << std::endl;
		g_system.Deinit();
		return 0;
	}
	Stage simpleStage;
	simpleStage.SetWindowSize(g_system.GetWidth(), g_system.GetHeight());
	simpleStage.SetShader(simpleProgram);
	static float delta = 0.0;
	while (!g_system.WindowIsClose())
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
		firstScript.mainLoop(delta);
		simpleStage.Render();
		g_system.Swap();
		g_system.PollEvent();
	}
	g_system.Deinit();
	return 0;
}
