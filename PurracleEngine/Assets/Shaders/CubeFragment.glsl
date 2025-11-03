#version 330 core 
in vec3 FragPos;
out vec4 FragColor;
void main() 
{ 
   vec3 normalizedPos = FragPos + vec3(0.5);
   vec3 color = normalizedPos;
   FragColor = vec4(color, 1.0);
}