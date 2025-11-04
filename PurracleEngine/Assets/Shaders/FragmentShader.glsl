#version 330 core

out vec4 FragColor;

in vec2 TextureCoord;
in vec3 Normal;

uniform sampler2D myTexture;

void main()
{
    FragColor = texture(myTexture, TextureCoord);
}