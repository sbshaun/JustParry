#version 330 core
out vec4 FragColor;

uniform bool isSelected;
uniform bool isHovered;
uniform bool isPressed;

void main()
{
    vec4 baseColor;
    
    if (isSelected) {
        baseColor = vec4(0.6, 0.6, 0.6, 0.5); // Dark and opaque when selected
        if (isHovered) {
            baseColor = vec4(0.7, 0.7, 0.7, 0.5); // Slightly lighter when selected and hovered
        }
        if (isPressed) {
            baseColor = vec4(0.5, 0.5, 0.5, 0.6); // Even darker when selected and pressed
        }
    } else {
        baseColor = vec4(1.0, 1.0, 1.0, 0.0); // Fully transparent by default
        if (isHovered) {
            baseColor = vec4(0.6, 0.6, 0.6, 0.3); // Much darker gray when hovered (changed from 0.8 to 0.6)
        }
        if (isPressed) {
            baseColor = vec4(0.5, 0.5, 0.5, 0.4); // Even darker when pressed (changed from 0.7 to 0.5)
        }
    }
    
    FragColor = baseColor;
} 