#include "ShaderProgram.hpp"
#include "Exception.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

ShaderProgram::ShaderProgram()
	: m_program(0)
	, m_vertexShader(0)
	, m_pixelShader(0)
	, m_geomShader(0)
	, m_projMatrixUniform(-1)
	, m_worldMatrixUniform(-1)
	, m_cameraMatrixUniform(-1)
	, m_vertexAttribute(-1)
	, m_texcoordAttribute(-1)
	, m_normalAttribute(-1)
	, m_tangentAttribute ( -1)
	, m_bitangentAttribute ( -1 )
	, m_colorAttribute(-1)
	, m_difuseTextureIdx(-1)
	, m_normalTextureIdx(-1)
	, m_isUse(false)
	, m_programReady(false)
	, m_projectionMatrixConstant(1.0)
	, m_cameraMatrixConstant(1.0)
	, m_normalMatrixConstant(1.0)
	, m_worldMatrixConstant(1.0)
	, m_counterOfUse(new char('a'))
{
	m_program = glCreateProgram();
}

ShaderProgram::ShaderProgram(const ShaderProgram &rhs)
	: m_program(rhs.m_program)
	, m_vertexShader(rhs.m_vertexShader)
	, m_pixelShader(rhs.m_pixelShader)
	, m_geomShader(rhs.m_geomShader)
	, m_projMatrixUniform(rhs.m_projMatrixUniform)
	, m_worldMatrixUniform(rhs.m_worldMatrixUniform)
	, m_cameraMatrixUniform(rhs.m_cameraMatrixUniform)
	, m_vertexAttribute(rhs.m_vertexAttribute)
	, m_texcoordAttribute(rhs.m_texcoordAttribute)
	, m_normalAttribute(rhs.m_normalAttribute)
	, m_colorAttribute(rhs.m_colorAttribute)
	, m_tangentAttribute ( rhs.m_tangentAttribute)
	, m_bitangentAttribute ( rhs.m_bitangentAttribute )
	, m_difuseTextureIdx(rhs.m_difuseTextureIdx)
	, m_normalTextureIdx(rhs.m_normalTextureIdx)
	, m_isUse(rhs.m_isUse)
	, m_programReady(rhs.m_programReady)
	, m_projectionMatrixConstant(rhs.m_projectionMatrixConstant)
	, m_cameraMatrixConstant(rhs.m_cameraMatrixConstant)
	, m_normalMatrixConstant(rhs.m_normalMatrixConstant)
	, m_worldMatrixConstant(rhs.m_worldMatrixConstant)
	, m_counterOfUse(rhs.m_counterOfUse)
{
}

ShaderProgram::~ShaderProgram()
{
	if (m_counterOfUse.use_count() == 1)
	{
		glDeleteProgram(m_program);
		if (m_vertexShader)
			glDeleteShader(m_vertexShader);
		if (m_pixelShader)
			glDeleteShader(m_pixelShader);
		if (m_geomShader)
			glDeleteShader(m_geomShader);
	}
}

void ShaderProgram::CreateProgramInternal(const std::string &path, GLuint *shader, GLuint shaderType)
{
	ifstream inputStream(path.c_str());
	string line;
	string program;
	if (inputStream)
	{
		while ( getline (inputStream,line) )
		{
			program.append(line+'\n');
		}
	}
	else
	{
		throw Exception("[ShaderProgram] could not open file: " + path);
	}

	*shader = glCreateShader(shaderType);
	const char *p = program.c_str();
	glShaderSource(*shader, 1, static_cast<const GLchar**>(&p), NULL);
	glCompileShader(*shader);
	GLint isCompiled = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &isCompiled);
	if (GL_FALSE == isCompiled)
	{
		GLint maxLength = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar *infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(*shader, maxLength, &maxLength, infoLog);
		string error(infoLog);
		delete [] infoLog;
		glDeleteShader(*shader);
		*shader = 0;
		throw Exception(error);
	}
}

void ShaderProgram::AttachVertexShader(const std::string &path)
{
	try
	{
		CreateProgramInternal(path, &m_vertexShader, GL_VERTEX_SHADER);
	}
	catch(const Exception &e)
	{
		throw Exception("[ShaderProgram] could not create vertex shader\n" + e.m_errorMsg);
	}
}

void ShaderProgram::AttachPixelShader(const std::string &path)
{
	try
	{
		CreateProgramInternal(path, &m_pixelShader, GL_FRAGMENT_SHADER);
	}
	catch(const Exception &e)
	{
		throw Exception("[ShaderProgram] could not create pixel shader\n" + e.m_errorMsg);
	}
}

void ShaderProgram::AttachGeometryShader(const std::string &path)
{
	throw Exception("[ShaderProgram] could not create geometry shader");
}

void ShaderProgram::CreateShaderProgram()
{
	if (!m_vertexShader || !m_pixelShader)
	{
		throw Exception("[ShaderProgram] there is no created any shaders yet");
	}
	if (m_vertexShader)
		glAttachShader(m_program, m_vertexShader);

	if (m_pixelShader)
		glAttachShader(m_program, m_pixelShader);

	if (m_geomShader)
		glAttachShader(m_program, m_geomShader);

	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");
	glBindAttribLocation(m_program, 3, "tangent");
	glBindAttribLocation(m_program, 4, "bitangent");
	glBindAttribLocation(m_program, 5, "color");

	glLinkProgram(m_program);
	GLint isLinked = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
	if (GL_FALSE == isLinked)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &infoLog[0]);
		cout << "len: " << infoLog.size() << endl;
		for (int i = 0; i < infoLog.size(); i++)
			cout << infoLog[i];
		throw Exception("[ShaderProgram] could not linked program");
	}

	m_projMatrixUniform = glGetUniformLocation(m_program, "projMatrix");
	m_worldMatrixUniform = glGetUniformLocation(m_program, "worldMatrix");
	m_cameraMatrixUniform = glGetUniformLocation(m_program, "cameraMatrix");
	m_normalMatrixUniform = glGetUniformLocation(m_program, "normalMatrix");

	m_difuseTextureIdx = glGetUniformLocation(m_program, "diffuseTexture");
	m_normalTextureIdx = glGetUniformLocation(m_program, "normalTexture");

	m_vertexAttribute = glGetAttribLocation(m_program, "position");
	m_texcoordAttribute = glGetAttribLocation(m_program, "texCoord");
	m_normalAttribute = glGetAttribLocation(m_program, "normal");
	m_colorAttribute = glGetAttribLocation(m_program, "color");
	m_tangentAttribute = glGetAttribLocation(m_program, "tangent");
	m_bitangentAttribute = glGetAttribLocation(m_program, "bitangent");
	m_programReady = true;
}

void ShaderProgram::UseProgram()
{
	if (m_programReady && !m_isUse)
	{
		m_isUse = true;
		glUseProgram(m_program);
	}
}

void ShaderProgram::ReleaseProgram()
{
	m_isUse = false;
	glUseProgram(0);
}

void ShaderProgram::SetWorldMatrixConstant(const glm::mat4 & world)
{
	m_worldMatrixConstant = world;
	if (m_worldMatrixUniform >= 0)
	{
		glUniformMatrix4fv(m_worldMatrixUniform, 1, GL_FALSE, &m_worldMatrixConstant[0][0]);
	}
}

void ShaderProgram::SetProjectionMatrixConstant(const glm::mat4 & proj)
{
	m_projectionMatrixConstant = proj;
	if (m_projMatrixUniform >= 0)
	{
		glUniformMatrix4fv(m_projMatrixUniform, 1, GL_FALSE, &m_projectionMatrixConstant[0][0]);
	}
}

void ShaderProgram::SetCameraMatrixConstant(const glm::mat4 & camera)
{
	m_cameraMatrixConstant = camera;
	if (m_cameraMatrixUniform >= 0)
	{
		glUniformMatrix4fv(m_cameraMatrixUniform, 1, GL_FALSE, &m_cameraMatrixConstant[0][0]);
	}
}

void ShaderProgram::SetNormalMatrixConstant(const glm::mat4 & normal)
{
	m_normalMatrixConstant = normal;
	if (m_normalMatrixUniform >= 0)
	{
		glUniformMatrix4fv(m_normalMatrixUniform, 1, GL_FALSE, &m_normalMatrixConstant[0][0]);
	}
}

void ShaderProgram::SetDiffuseTexture(GLuint texture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(m_difuseTextureIdx, 0);
}

void ShaderProgram::SetNormalTexture(GLuint texture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(m_normalTextureIdx, 1);

}

