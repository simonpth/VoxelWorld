#include "raytraversal.h"

RayHit RayTraversal::traverse(const Ray &ray, float maxDistance, World *world) {
  glm::vec3 cPos = ray.origin;

  glm::ivec3 prevBlockPos = glm::floor(cPos);

  while (true) {
    glm::vec3 blockPos = glm::floor(cPos);
    if (world->getBlock(blockPos).isSolid()) {
      RayHit hit;
      hit.hit = true;
      hit.blockPos = glm::ivec3(blockPos);
      hit.blockBeforePos = prevBlockPos;
      hit.hitPoint = cPos;
      return hit;
    }

    prevBlockPos = glm::ivec3(blockPos);
    cPos += ray.direction * 0.01f;
    if (glm::length(cPos - ray.origin) > maxDistance) {
      break;
    }
  }

  RayHit miss;
  miss.hit = false;
  return miss;
}