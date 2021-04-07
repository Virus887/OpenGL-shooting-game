#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
in vec3 LightingColor; 

uniform sampler2D texture;


void main()
{
    FragColor = texture(texture, TexCoord) * vec4(LightingColor, 1.0);
    //FragColor = vec4(LightingColor,1.0);
}