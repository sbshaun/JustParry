#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;  // Model transformation matrix
uniform mat4 view;   // View matrix
uniform mat4 projection;  // Projection matrix
uniform vec3 playerPosition;  // Pass player position as uniform

void main() {
    vec3 newHitboxPosition = playerPosition + aPos;

    vec4 worldPos = vec4(newHitboxPosition, 1.0);

    // TODO: Apply projection and view matrices later
    gl_Position = worldPos;
}