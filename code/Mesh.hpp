#ifndef _MESH_HPP_
#define _MESH_HPP_

#include <GL/glew.h>
#include <iostream>
#include "Material.hpp"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh(const aiMesh *mesh, const aiScene *sc, const std::string &root_path);
	~Mesh();
	void SetMaterial(const Material &material);
	Material& GetMaterial() {return m_material;}
	glm::mat4 GetWorldMatrix();
	void Render();
private:
	enum buffers{
		vertexBuffer,
		textureBuffer,
		colorBuffer,
		normalBuffer
	};
	GLuint m_vao;
	GLuint m_vertexVbo;
	GLuint m_texcoordVbo;
	GLuint m_normalsVbo;
	GLuint m_indexVbo;
	GLuint m_tangentVbo;
	GLuint m_bitangentVbo;
	unsigned int m_numberIndices;
	Material m_material;
};

#endif //_MESH_HPP_
