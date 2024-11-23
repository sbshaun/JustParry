#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 FragColor;

void main() {
    float distance = length(TexCoords - vec2(0.5, 0.5));
    
    if (distance > 0.5) {
        discard;
    }

    FragColor = ParticleColor;
}