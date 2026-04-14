#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>

struct Plane {
  glm::vec3 normal;
  float d;
};

struct Frustum {
  Plane planes[6]; // left, right, bottom, top, near, far

  void extractFrustum(const glm::mat4 &vp) {
    // GLM is column-major, so rows are accessed as vp[col][row]
    // We extract rows of the matrix manually
    glm::vec4 rows[4] = {
        {vp[0][0], vp[1][0], vp[2][0], vp[3][0]}, // row 0
        {vp[0][1], vp[1][1], vp[2][1], vp[3][1]}, // row 1
        {vp[0][2], vp[1][2], vp[2][2], vp[3][2]}, // row 2
        {vp[0][3], vp[1][3], vp[2][3], vp[3][3]}, // row 3
    };

    auto toPlane = [](glm::vec4 v) -> Plane {
      float len = glm::length(glm::vec3(v));
      return {glm::vec3(v) / len, v.w / len};
    };

    planes[0] = toPlane(rows[3] + rows[0]); // left
    planes[1] = toPlane(rows[3] - rows[0]); // right
    planes[2] = toPlane(rows[3] + rows[1]); // bottom
    planes[3] = toPlane(rows[3] - rows[1]); // top
    planes[4] = toPlane(rows[3] + rows[2]); // near
    planes[5] = toPlane(rows[3] - rows[2]); // far
  }

  bool aabbInFrustum(glm::vec3 min, glm::vec3 max) {
    for (const auto &plane : planes) {
      // Pick the corner most in the direction of the plane normal (positive vertex)
      glm::vec3 positive = {
          plane.normal.x >= 0 ? max.x : min.x,
          plane.normal.y >= 0 ? max.y : min.y,
          plane.normal.z >= 0 ? max.z : min.z,
      };
      if (glm::dot(plane.normal, positive) + plane.d < 0)
        return false; // fully outside
    }
    return true;
  }
};

#endif // FRUSTUM_H