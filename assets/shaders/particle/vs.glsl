#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;
layout (location = 2) in vec4 aColor;

uniform mat4 worldModel;

out vec4 particleColor;

void main() {
    vec3 worldPos = (worldModel * vec4(aPos * 0.02 + aOffset, 1.0)).xyz;
    gl_Position = vec4(worldPos, 1.0);
    particleColor = aColor;
}