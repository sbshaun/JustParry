#version 330 core
out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D birdTexture;

void main() {
    fragColor = texture(birdTexture, TexCoord);  // bird Sprite
}