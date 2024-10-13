#version 330 core
out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D m_bird_texture;

void main() {
    fragColor = texture(m_bird_texture, TexCoord);  // bird Sprite
}