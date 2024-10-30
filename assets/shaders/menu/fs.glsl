#version 330 core
out vec4 FragColor;  // Output color of the fragment

in vec2 TexCoord;    // The texture coordinates passed from the vertex shader

uniform sampler2D texture1; // The texture sampler

void main()
{
    FragColor = texture(texture1, TexCoord); // Sample the texture and output the color
}
