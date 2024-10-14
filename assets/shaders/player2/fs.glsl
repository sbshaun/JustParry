#version 330 core
out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D m_bird_texture;
uniform bool takenDamage;

void main() {
    if (!takenDamage) {
        fragColor = texture(m_bird_texture, TexCoord);  // bird Sprite
    } else {
        vec4 textureColor = texture(m_bird_texture, TexCoord);  // Sample the texture color
        vec4 redTint = vec4(1.0, 0.8, 0.8, 1.0);  // Slight red tint (1.0 in red, 0.8 in green/blue)
        fragColor = textureColor * redTint;  // Apdwply the red tint to the texture
    }
    
}