#include "blockregistrytbo.h"

BlockRegistryTBO::BlockRegistryTBO() {}

void BlockRegistryTBO::initialize(const BlockRegistry &registry) {
  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_TEXTURE_BUFFER, m_bufferId);
  glBufferData(GL_TEXTURE_BUFFER, registry.getBlockTextures().size() * sizeof(BlockTexture),
               registry.getBlockTextures().data(), GL_STATIC_DRAW);

  glGenTextures(1, &m_textureId);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureId);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_bufferId);

  glBindTexture(GL_TEXTURE_BUFFER, 0); // Clean up
  glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void BlockRegistryTBO::bind(GLuint textureUnit) const {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureId);
}

void BlockRegistryTBO::deleteBuffer() {
  glDeleteBuffers(1, &m_bufferId);
  glDeleteTextures(1, &m_textureId);
}