#version 330 core
out vec4 FragColor;

uniform vec3 buttonColor;
uniform vec2 buttonSize;
uniform vec2 buttonPos;
uniform float cornerSize = 30.0;
uniform float borderThickness = 6.0;

void main()
{
    // Get position relative to button's top-left corner
    vec2 pixelPos = gl_FragCoord.xy - buttonPos;
    
    // Check if point is inside button bounds
    if (pixelPos.x < 0 || pixelPos.x > buttonSize.x || 
        pixelPos.y < 0 || pixelPos.y > buttonSize.y) {
        discard;
    }
    
    // Calculate distance from corners with reduced tilt on left/right edges
    vec2 fromCorner;
    if (pixelPos.x < cornerSize) {
        fromCorner.x = (cornerSize - pixelPos.x) * 0.6;
    } else if (pixelPos.x > buttonSize.x - cornerSize) {
        fromCorner.x = (pixelPos.x - (buttonSize.x - cornerSize)) * 0.6;
    } else {
        fromCorner.x = 0;
    }
    
    if (pixelPos.y < cornerSize) {
        fromCorner.y = cornerSize - pixelPos.y;
    } else if (pixelPos.y > buttonSize.y - cornerSize) {
        fromCorner.y = pixelPos.y - (buttonSize.y - cornerSize);
    } else {
        fromCorner.y = 0;
    }
    
    // Check if point should be discarded (outside the octagon)
    if (fromCorner.x + fromCorner.y > cornerSize) {
        discard;
    }
    
    // Check if we're in the border area
    bool isBorder = 
        pixelPos.x < borderThickness || // Left border
        pixelPos.x > buttonSize.x - borderThickness || // Right border
        pixelPos.y < borderThickness || // Bottom border
        pixelPos.y > buttonSize.y - borderThickness || // Top border
        (fromCorner.x + fromCorner.y > cornerSize - borderThickness); // Corner borders
    
    // Set color based on whether we're drawing the border or the button
    if (isBorder) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black border
    } else {
        vec3 fillColor = vec3(1.0, 1.0, 1.0); // White fill
        if (buttonColor != vec3(1.0)) { // If button color is not white (hover/press state)
            fillColor = mix(fillColor, buttonColor, 0.3); // Blend with button color for hover effect
        }
        FragColor = vec4(fillColor, 1.0);
    }
} 