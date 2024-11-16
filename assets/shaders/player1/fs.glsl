#version 330 core
out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D m_bird_texture;
uniform bool takenDamage;
uniform bool isParrying; 
uniform bool isBlockStunned;
uniform bool stunned;

void main() {
    vec4 tint = vec4(1.0, 0.8, 0.8, 1.0);  // Slight red tint (1.0 in red, 0.8 in green/blue)

    if (takenDamage) {
        vec4 textureColor = texture(m_bird_texture, TexCoord);  // Sample the texture color
        fragColor = textureColor * tint;  // Apply the red tint to the texture
        return;
    } 

    if (stunned) {
        vec4 textureColor = texture(m_bird_texture, TexCoord);  
        fragColor = textureColor * vec4(0.7, 0.7, 0.7, 1.0);  // grey tint 
        return;
    }

    if (isParrying) {
        tint = vec4(0.8, 0.8, 0.8, .2);  // Slight green tint (0.8 in red, 1.0 in green, 0.8 in blue)
        fragColor = texture(m_bird_texture, TexCoord) * tint;  // Apply the green tint to the texture
        return;
    }

    if (isBlockStunned) {
        tint = vec4(0.8, 1.0, 0.8, .2);  // Slight green tint (0.8 in red, 1.0 in green, 0.8 in blue)
        fragColor = texture(m_bird_texture, TexCoord) * tint;  // Apply the green tint to the texture
        return;
    }

    fragColor = texture(m_bird_texture, TexCoord);  // bird Sprite
}