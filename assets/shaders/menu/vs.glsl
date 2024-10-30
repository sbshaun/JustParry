#version 330 core
layout (location = 0) in vec3 aPos;       // The position variable
layout (location = 1) in vec2 aTexCoord;  // The texture coordinate variable

out vec2 TexCoord; // Pass the texture coordinate to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // Set the position in clip space
    TexCoord = aTexCoord;          // Pass the texture coordinates to the fragment shader
}
