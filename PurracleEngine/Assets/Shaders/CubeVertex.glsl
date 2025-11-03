#version 330 core

layout(location = 0) in vec3 aPos;   
layout(location = 1) in vec3 aColor; 

out vec3 FragPos;   

uniform mat4 transform;

void main()
{
    FragPos = aPos;
    gl_Position = transform * vec4(aPos, 1.0);
}