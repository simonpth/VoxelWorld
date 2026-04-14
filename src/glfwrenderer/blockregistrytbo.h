#ifndef BLOCKREGISTRYTBO_H
#define BLOCKREGISTRYTBO_H

#include "engine/data/blockregistry.h"

#include <glad/glad.h>

class BlockRegistryTBO {
public:
  BlockRegistryTBO();

  void initialize(const BlockRegistry &registry);

  void bind(GLuint textureUnit) const;

  void deleteBuffer();
private:
  GLuint m_bufferId;
  GLuint m_textureId;
};

#endif // BLOCKREGISTRYTBO_H