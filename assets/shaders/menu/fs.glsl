#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform float brightness = 1.0;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(texColor.rgb * brightness, texColor.a);
}
