#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include <glad/glad.h>
#include <stb_image/stb_image.h>

class TextureAtlas {
public:
  TextureAtlas();

  void initialize(const char *filePath);

  void bind(unsigned int slot = 0) const;

  void cleanup();

private:
  GLuint m_textureId;
};

#endif // TEXTUREATLAS_H