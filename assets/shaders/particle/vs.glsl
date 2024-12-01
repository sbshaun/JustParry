#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;
layout (location = 2) in vec4 aColor;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 worldModel;

void main() {
    TexCoords = aPos.xy + vec2(0.5, 0.5); // Map vertex positions to texture coordinates
    ParticleColor = aColor;
    gl_Position = worldModel * vec4(aPos * 0.02 + aOffset, 1.0);
}