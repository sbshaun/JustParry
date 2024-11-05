#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D texture1;
uniform float brightness;
uniform float alpha;

void main()
{
    if (textureSize(texture1, 0).x > 0) {
        vec4 texColor = texture(texture1, TexCoord);
        FragColor = vec4(texColor.rgb * brightness, texColor.a * alpha);
    } else {
        FragColor = vec4(color, 1.0);
    }
}
