
#include "Model.hpp"
#include "Material.hpp"

std::string SplitFilename (const std::string& str)
{
  unsigned found = str.find_last_of("/\\");
  return str.substr(0,found);
}

Model::Model(const std::string & path)
{
	const struct aiScene* scene = NULL;
	scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene)
	{
		return;
	}
	std::string root_path = SplitFilename(path);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh *mesh = new Mesh(scene->mMeshes[i], scene, root_path);
		m_meshConteiner.push_back(mesh);
	}
	aiReleaseImport(scene);
}

Model::~Model()
{
	for (unsigned int i = 0; i < m_meshConteiner.size(); i++)
	{
		delete m_meshConteiner[i];
	}
	m_meshConteiner.clear();
}

void Model::Render()
{
	for (unsigned int i = 0; i < m_meshConteiner.size(); i++)
	{
		m_meshConteiner[i]->Render();
	}
}
