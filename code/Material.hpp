#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "Exception.hpp"
#include <GL/glew.h>
#include <string>
//#include "Ref.hpp"
#include <memory>
#include "ShaderProgram.hpp"

class Material
{
public:
	Material();
	Material(const Material &rhs);
	Material& operator = (const Material &rhs);
	~Material();
	void SetDiffuseTexture(const std::string &path);
	void SetNormalTexture(const std::string &path);
	GLuint GetDiffuseTexture() const {return *m_diffuseTexture;}
	GLuint GetNormalTexture() const {return *m_normalTexture;}
	void SetShader(const ShaderProgram& shader) {m_shader = shader;}
	ShaderProgram& GetShader() {return m_shader;}
private:
	enum TextureType
	{
		diffuseTexture,
		normalTexture
	};
	void CreateTextureInternal(const std::string &path, GLuint *texture, TextureType type);
	std::shared_ptr<GLuint> m_diffuseTexture;
	std::shared_ptr<GLuint> m_normalTexture;
	ShaderProgram m_shader;
};

#endif //_MATERIAL_HPP_
