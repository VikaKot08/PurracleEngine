#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 eyePosition = vec4(0.0, 5.0, 0.0, 0.0);

out vec2 TextureCoord;
out vec3 Normal;
out vec3 VertexPosition;
out vec3 VecToEye;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
    
    TextureCoord = aTextureCoord;
    
    // Transform normal to world space using normal matrix (inverse transpose)
    Normal = mat3(transpose(inverse(transform))) * aNormal;

    VertexPosition = (transform * vec4(aPos, 1.0)).xyz;
    VecToEye = eyePosition.xyz - VertexPosition;
}
