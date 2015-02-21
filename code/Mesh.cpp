#include "Mesh.hpp"
#include "Exception.hpp"
#include <iostream>

using namespace std;

Mesh::Mesh(const aiMesh *mesh, const aiScene *sc, const std::string &root_path)
	: m_vao(-1)
	, m_vertexVbo(-1)
	, m_texcoordVbo(-1)
	, m_normalsVbo(-1)
	, m_tangentVbo( -1)
	, m_bitangentVbo( -1)
	, m_indexVbo(-1)
	, m_numberIndices(0)
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_numberIndices = mesh->mNumFaces * 3;

	if (mesh->HasPositions())
	{
		float *vert = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			vert[i * 3] = mesh->mVertices[i].x;
			vert[i * 3 + 1] = mesh->mVertices[i].y;
			vert[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &m_vertexVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(float), vert, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		delete [] vert;
	}

	if (mesh->HasTextureCoords(0))
	{

		float *tex = new float [mesh->mNumVertices * 2];
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			tex[i * 2] = mesh->mTextureCoords[0][i].x;
			tex[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &m_texcoordVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_texcoordVbo);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(float), tex, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		delete [] tex;
	}
	if (mesh->HasNormals())
	{
		float *norm = new float [mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			norm[i * 3] = mesh->mNormals[i].x;
			norm[i * 3 + 1] = mesh->mNormals[i].y;
			norm[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &m_normalsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalsVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(float), norm, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
		delete [] norm;
	}

	if (mesh->HasTangentsAndBitangents())
	{
		float *tang = new float [mesh->mNumVertices * 3];
		float *bitang = new float [mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			tang[i * 3] = mesh->mTangents[i].x;
			tang[i * 3 + 1] = mesh->mTangents[i].y;
			tang[i * 3 + 2] = mesh->mTangents[i].z;

			bitang[i * 3] = mesh->mBitangents[i].x;
			bitang[i * 3 + 1] = mesh->mBitangents[i].y;
			bitang[i * 3 + 2] = mesh->mBitangents[i].z;
		}

		glGenBuffers(1, &m_tangentVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_tangentVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(float), tang, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);


		glGenBuffers(1, &m_bitangentVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_bitangentVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(float), bitang, GL_STATIC_DRAW);

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(4);

		delete [] tang;
		delete [] bitang;
	}


	if (mesh->HasFaces())
	{
		unsigned int *indices = new unsigned int [mesh->mNumFaces * 3];
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &m_indexVbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		delete [] indices;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];
	aiString texPath;
	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
	{
		std::string path = root_path + "/" + texPath.data;
		m_material.SetDiffuseTexture(path.c_str());
	}

	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &texPath))
	{
		std::string path = root_path + "/" + texPath.data;
		m_material.SetNormalTexture(path.c_str());
	}
	else
	{
		m_material.SetNormalTexture("data/def_normal.jpg");
	}

}

Mesh::~Mesh()
{
	if (m_vertexVbo > -1)
		glDeleteBuffers(1, &m_vertexVbo);

	if (m_texcoordVbo > -1)
			glDeleteBuffers(1, &m_texcoordVbo);

	if (m_normalsVbo > -1)
			glDeleteBuffers(1, &m_normalsVbo);

	if (m_indexVbo > -1)
			glDeleteBuffers(1, &m_indexVbo);

	if (m_tangentVbo > -1)
			glDeleteBuffers(1, &m_tangentVbo);

	if (m_bitangentVbo > -1)
			glDeleteBuffers(1, &m_bitangentVbo);

	glDeleteVertexArrays(1, &m_vao);
}

void Mesh::SetMaterial(const Material &material)
{
	m_material = material;
}

void Mesh::Render()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numberIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
