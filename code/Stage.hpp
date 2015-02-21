#ifndef _STAGE_HPP_
#define _STAGE_HPP_

#include <glm/glm.hpp>
#include "ShaderProgram.hpp"
#include <vector>
#include "Model.hpp"

class Stage
{
public:
	Stage();
	~Stage();
	void SetWorldMatrixConstant(const glm::mat4 &world);
	void SetProjectionMatrixConstant(const glm::mat4 &proj);
	void SetCameraMatrixConstant(const glm::mat4 &camera);
	void SetShader(const ShaderProgram& shader);
	void SetWindowSize(int width, int height) {m_width = width; m_height = height;}
	void AddModelToStage(Model* mesh) {m_modelContainer.push_back(mesh);}
	void Render();
	//void SetShader(const ShaderProgram &program);
private:
	ShaderProgram m_currentShader;
	glm::mat4 m_worldMatrixConstant;
	glm::mat4 m_projectionMatrixConstant;
	glm::mat4 m_cameraMatrixConstant;
	int m_width;
	int m_height;
	std::vector<Model*> m_modelContainer;
};

#endif //_STAGE_HPP_
