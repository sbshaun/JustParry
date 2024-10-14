#version 330 core
layout (location = 0) in vec3 aPos;

// uniform mat4 model;  // Model transformation matrix
// uniform mat4 view;   // View matrix
// uniform mat4 projection;  // Projection matrix

void main() {
    gl_Position = vec4(aPos, 1.0);
}