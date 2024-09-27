#include "texture.h"

using namespace jisaacs;

Texture2D::Texture2D(const char* filePath, int filterMode, int wrapMode) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	unsigned char* data = stbi_load(filePath, &m_width, &m_height, &m_nrChannels, 0);
	if (data)
	{
		int format;
		if (m_nrChannels == 4) {
			format = GL_RGBA;
		}
		else {
			format = GL_RGB;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_set_flip_vertically_on_load(true);
	stbi_image_free(data);
}

Texture2D::~Texture2D() {

}

void Texture2D::Bind(unsigned int slot) {
	glActiveTexture(slot); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_id);
}

unsigned int Texture2D::getTextureID() {
	return m_id;
}