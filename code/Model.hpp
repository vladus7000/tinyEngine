#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <iostream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.hpp"
class Model
{
public:
	Model(const std::string & path);
	~Model();

	glm::mat4 GetWorldMatrix(){ return m_worldMatrix; }
	void SetWorldMatrix(glm::mat4 &matrix) {m_worldMatrix = matrix;}
	void Render();
	Mesh* GetI(unsigned int i) {return m_meshConteiner[i];}
	unsigned int GetSize() {return m_meshConteiner.size();}
private:
	glm::mat4 m_worldMatrix;
	std::vector<Mesh*> m_meshConteiner;
};
#endif //_MODEL_HPP_
