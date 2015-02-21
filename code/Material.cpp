#include "Material.hpp"
#include <FreeImage.h>

Material::Material()
	: m_diffuseTexture(new GLuint(-1))
	, m_normalTexture(new GLuint(-1))
{
}

Material::Material(const Material &rhs)
	: m_diffuseTexture(rhs.m_diffuseTexture)
	, m_normalTexture(rhs.m_normalTexture)
{
}

Material& Material::operator = (const Material &rhs)
{
	m_diffuseTexture = rhs.m_diffuseTexture;
	m_normalTexture = rhs.m_normalTexture;
	m_shader = rhs.m_shader;
	return *this;
}

Material::~Material()
{
	if (m_diffuseTexture.use_count() == 1  && *m_diffuseTexture > -1)
		glDeleteTextures(1, m_diffuseTexture.get());

	if (m_normalTexture.use_count() == 1 && *m_normalTexture > -1)
		glDeleteTextures(1, m_normalTexture.get());
}

void Material::SetDiffuseTexture(const std::string &path)
{
	try
	{
		CreateTextureInternal(path, m_diffuseTexture.get(), diffuseTexture);
	}
	catch(const Exception &e)
	{
		throw Exception("[SetDiffuseTexture] could not create: " + path + "\n" + e.m_errorMsg);
	}
}

void Material::SetNormalTexture(const std::string &path)
{
	try
	{
		CreateTextureInternal(path, m_normalTexture.get(), normalTexture);
	}
	catch(const Exception &e)
	{
		throw Exception("[SetNormalTexture] could not create: " + path + "\n" + e.m_errorMsg);
	}
}

void Material::CreateTextureInternal(const std::string &path, GLuint *texture, TextureType type)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	int width(0), height(0);
	fif = FreeImage_GetFileType(path.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		throw Exception("[Loader] unknown type");
	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, path.c_str());

	//if the image failed to load, return failure
	if(!dib)
	{
		throw Exception("[Loader] don't supported format");
	}
	//retrieve the image data
	unsigned char *data = NULL;
	data = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((data == 0) || (width == 0) || (height == 0))
		throw Exception("[Loader] smth go wrong");
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	FreeImage_Unload(dib);
}
