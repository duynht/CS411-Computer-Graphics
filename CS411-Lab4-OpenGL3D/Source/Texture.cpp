#include "Texture.h"

Texture::Texture(const std::string& fileName) {
	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_RGBA);

	if (imageData == nullptr) {
		std::cerr << "Loading failed for texture: " << fileName << std::endl;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (imageData != nullptr) {
		SOIL_free_image_data(imageData);
		std::cout << "Loaded and freed texture data successfully!";
	}
}

Texture::~Texture() {
	glDeleteTextures(1, &texture);
}

void Texture::Bind(unsigned int unit) {
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}