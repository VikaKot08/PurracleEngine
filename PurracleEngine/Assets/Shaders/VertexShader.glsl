#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 TextureCoord;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0);

    TextureCoord = aTextureCoord;
    Normal = aNormal;
}
