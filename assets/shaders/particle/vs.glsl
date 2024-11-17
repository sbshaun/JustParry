#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;
layout (location = 2) in vec4 aColor;

out vec4 particleColor;

void main() {
    vec3 pos = aPos * 0.02 + aOffset; // Scale quad size
    gl_Position = vec4(pos, 1.0);
    particleColor = aColor;
}