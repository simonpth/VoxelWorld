#version 330 core

flat in uint blockIDOut;

out vec4 FragColor;

vec3 blockColors[3] = vec3[](
    vec3(1.0, 0.0, 0.0), // block ID 0: red
    vec3(0.0, 1.0, 0.0), // block ID 1: green
    vec3(0.0, 0.0, 1.0) // block ID 2: blue
    // ... add more colors for other block IDs as needed
);

void main() {
    FragColor = vec4(blockColors[blockIDOut], 1.0);
}
