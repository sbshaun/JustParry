#version 330 core
out vec4 FragColor;

in vec2 FragPos;

uniform vec3 buttonColor;
uniform float alpha = 1.0;
uniform vec2 buttonSize;
uniform float cornerRadius = 10.0;

void main()
{
    // Calculate distance from edges
    vec2 center = buttonSize * 0.5;
    vec2 dist = abs(FragPos - center);
    
    // Calculate corner radius in pixels
    float radius = min(cornerRadius, min(buttonSize.x, buttonSize.y) * 0.5);
    
    // Calculate distance to edge of rounded rectangle
    vec2 cornerTest = max(dist - (buttonSize * 0.5 - radius), 0.0);
    float dist2Corner = length(cornerTest);
    
    if (dist2Corner > radius) {
        // Outside the rounded rectangle
        discard;
    }
    
    // Smooth edge for anti-aliasing
    float edge = radius - 1.0;
    float smoothness = smoothstep(radius, edge, dist2Corner);
    
    FragColor = vec4(buttonColor, alpha * smoothness);
} 