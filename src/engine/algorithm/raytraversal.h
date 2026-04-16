#ifndef RAYTRAVERSAL_H
#define RAYTRAVERSAL_H

#include <glm/glm.hpp>
#include "engine/data/world.h"

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct RayHit {
  bool hit;
  glm::ivec3 blockPos; // World position of the block that was hit
  glm::ivec3 blockBeforePos; // World position of the block before the hit block (useful for placing blocks)
  glm::vec3 hitPoint;  // Exact point of intersection in world coordinates
  int face;            // Which face of the block was hit (0-5)
};

class RayTraversal {
public:
  static RayHit traverse(const Ray& ray, float maxDistance, World *world);
};
#endif // RAYTRAVERSAL_H