#include "textureatlas.h"

#include <iostream>

TextureAtlas::TextureAtlas() {}

void TextureAtlas::initialize(const char *filePath) {
  glGenTextures(1, &m_textureId);
  glBindTexture(GL_TEXTURE_2D, m_textureId);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Load image data
  stbi_set_flip_vertically_on_load(false);
  int width, height, channels;
  unsigned char *data = stbi_load(filePath, &width, &height, &channels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::println(std::cout, "Failed to load texture atlas: %s\n", filePath);
  }
  stbi_image_free(data);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void TextureAtlas::cleanup() {
  glDeleteTextures(1, &m_textureId);
}
