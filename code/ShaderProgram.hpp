#ifndef _SHADER_PROGRA_HPP_
#define _SHADER_PROGRA_HPP_

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <memory>

using namespace glm;

class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(const ShaderProgram &rhs);
	~ShaderProgram();
	void AttachVertexShader(const std::string &path);
	void AttachPixelShader(const std::string &path);
	void AttachGeometryShader(const std::string &path);
	void CreateShaderProgram();
	void UseProgram();
	bool isUse() const {return m_isUse;}
	void ReleaseProgram();
	void SetWorldMatrixConstant(const glm::mat4 &world);
	void SetProjectionMatrixConstant(const glm::mat4 &proj);
	void SetCameraMatrixConstant(const glm::mat4 & camera);
	void SetNormalMatrixConstant(const glm::mat4 & normal);
	void SetDiffuseTexture(GLuint texture);
	void SetNormalTexture(GLuint texture);
	bool isReady() {return m_programReady;}
private:
	void CreateProgramInternal(const std::string &path, GLuint *shader, GLuint shaderType);
	GLuint m_program;
	GLuint m_vertexShader;
	GLuint m_pixelShader;
	GLuint m_geomShader;
	GLint m_projMatrixUniform;
	GLint m_worldMatrixUniform;
	GLint m_cameraMatrixUniform;
	GLint m_normalMatrixUniform;
	GLint m_vertexAttribute;
	GLint m_texcoordAttribute;
	GLint m_normalAttribute;
	GLint m_colorAttribute;
	GLint m_difuseTextureIdx;
	GLint m_normalTextureIdx;
	GLint m_tangentAttribute;
	GLint m_bitangentAttribute;
	bool m_isUse;
	bool m_programReady;
	glm::mat4 m_worldMatrixConstant;
	glm::mat4 m_projectionMatrixConstant;
	glm::mat4 m_cameraMatrixConstant;
	glm::mat4 m_normalMatrixConstant;
	std::shared_ptr<char> m_counterOfUse;
};

#endif //_SHADER_PROGRA_HPP_
