#version 330 core
out vec4 FragColor;

uniform bool isSelected;
uniform bool isHovered;
uniform bool isPressed;
uniform vec3 tintColor;

void main()
{
    vec3 baseColor = tintColor;
    float alpha = 0.8;

    if (isHovered) {
        baseColor = baseColor * 1.2;  // Lighten when hovered
    }
    if (isSelected) {
        baseColor = baseColor * 0.8;  // Darken when selected
        alpha = 0.9;
    }
    if (isPressed) {
        baseColor = baseColor * 0.7;  // Darken more when pressed
    }

    FragColor = vec4(baseColor, alpha);
} 