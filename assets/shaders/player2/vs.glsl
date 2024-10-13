#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;  // Model transformation matrix
// uniform mat4 view;   // View matrix
// uniform mat4 projection;  // Projection matrix

void main() {
    gl_Position = model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}