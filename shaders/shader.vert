#version 330 core
layout(location = 0) in vec3 vertices;
uniform mat4 mvp_matrix;

void main() {
    gl_Position = mvp_matrix * vec4(vertices, 1.0);
}
