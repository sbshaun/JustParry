#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;  // Model transformation matrix
// uniform mat4 view;   // View matrix
// uniform mat4 projection;  // Projection matrix

out vec2 TexCoord;
out vec3 WorldPosition;

void main() {
    // Apply transformations
    vec4 worldPos = model * vec4(aPos, 1.0);

    // Output the world position to pass it to the hitbox shader
	WorldPosition = worldPos.xyz;

    // Compute final vertex position. TODO: Apply project and view matrices later
    gl_Position = worldPos;

    TexCoord = aTexCoord;
}