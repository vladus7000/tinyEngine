#include "Stage.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

Stage::Stage()
	: m_width(0)
	, m_height(0)
	, m_worldMatrixConstant(1.0)
	, m_projectionMatrixConstant(1.0)
	, m_cameraMatrixConstant(1.0)
{

}

Stage::~Stage()
{
	m_modelContainer.clear();
}

void Stage::SetWorldMatrixConstant(const glm::mat4 &world)
{
	m_worldMatrixConstant = world;
}

void Stage::SetProjectionMatrixConstant(const glm::mat4 &proj)
{
	m_projectionMatrixConstant = proj;
}

void Stage::SetCameraMatrixConstant(const glm::mat4 &camera)
{
	m_cameraMatrixConstant = camera;
}

void Stage::SetShader(const ShaderProgram& shader)
{
	m_currentShader = shader;
	m_currentShader.UseProgram();
}

void Stage::Render()
{
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.2, 0.2, 0.2, 1.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	for (unsigned int i = 0; i < m_modelContainer.size(); i++)
	{
		glm::mat4 worldMatrix = m_modelContainer[i]->GetWorldMatrix();
		for (unsigned int j = 0; j < m_modelContainer[i]->GetSize(); j++)
		{
			Mesh *currentMesh = m_modelContainer[i]->GetI(j);
			Material &m = currentMesh->GetMaterial();

			//ShaderProgram &s = m.GetShader();
			//s.UseProgram();
			if (m_currentShader.isUse())
			{
				glm::mat4 norm1 = glm::inverse(worldMatrix);
				glm::mat4 norm = glm::transpose(norm1);
				m_currentShader.SetDiffuseTexture(m.GetDiffuseTexture());
				m_currentShader.SetNormalTexture(m.GetNormalTexture());
				m_currentShader.SetCameraMatrixConstant(m_cameraMatrixConstant);
				m_currentShader.SetProjectionMatrixConstant(m_projectionMatrixConstant);
				m_currentShader.SetWorldMatrixConstant(worldMatrix);
				m_currentShader.SetNormalMatrixConstant(norm);
				currentMesh->Render();
			}
		}
	}
}
