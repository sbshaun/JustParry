#version 330 core
out vec4 FragColor;

uniform vec3 borderColor;
uniform vec3 fillColor;
uniform float borderThickness; // This will be a fraction of the rectangle's size

void main()
{
    // Get the pixel position in normalized coordinates
    vec2 pixelPos = gl_FragCoord.xy / vec2(1280.0, 720.0); // Assuming window size is 1280x720
    pixelPos = pixelPos * 2.0 - 1.0; // Convert to NDC (-1 to 1)

    // Calculate the border area based on the normalized coordinates
    float borderWidth = borderThickness; // Use the uniform directly

    // Check if the pixel is within the border area
    if (abs(pixelPos.x) > (1.0 - borderWidth) || abs(pixelPos.y) > (1.0 - borderWidth))
    {
        FragColor = vec4(borderColor, 1.0); // Set border color
    }
    else
    {
        FragColor = vec4(fillColor, 1.0); // Set fill color
    }
}