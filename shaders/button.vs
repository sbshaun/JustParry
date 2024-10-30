#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 FragPos;

uniform vec2 buttonSize;

void main() {
    gl_Position = vec4(aPos, 1.0);
    // Pass the fragment position in screen coordinates
    FragPos = (aPos.xy + 1.0) * 0.5 * buttonSize;
} 