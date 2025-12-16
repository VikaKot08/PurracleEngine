#version 330 core

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

struct Light {
    int type;                    
    vec3 position;               // For point/spot lights
    vec3 direction;              // For directional/spot lights
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;            
    float cutoffAngle;           
    float outerCutoffAngle;    
    bool enabled;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec4 MaterialAmbient = vec4(1, 1, 1, 1);
uniform vec4 MaterialDiffuse = vec4(1, 1, 1, 1);
uniform vec4 MaterialSpecular = vec4(1, 1, 1, 1);
uniform int MaterialShine = 8;

out vec4 FragColor;

in vec2 TextureCoord;
in vec3 Normal;
in vec3 VertexPosition;
in vec3 VecToEye;

uniform sampler2D myTexture;

vec3 calculateLight(Light light, vec3 normal, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir;
    float attenuation = 1.0;
    
    if (light.type == LIGHT_DIRECTIONAL) {
        lightDir = normalize(-light.direction);
    } else {
        lightDir = normalize(light.position - VertexPosition);
        
        float distance = length(light.position - VertexPosition);
        attenuation = 1.0 / (light.attenuation.x + 
                            light.attenuation.y * distance + 
                            light.attenuation.z * distance * distance);
        
        if (light.type == LIGHT_SPOT) {
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = cos(radians(light.cutoffAngle)) - cos(radians(light.outerCutoffAngle));
            float intensity = clamp((theta - cos(radians(light.outerCutoffAngle))) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
            
            // Early exit if outside cone
            if (theta < cos(radians(light.outerCutoffAngle))) {
                return vec3(0.0);
            }
        }
    }
    
    vec3 ambient = light.ambient.rgb * MaterialAmbient.rgb * baseColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse.rgb * MaterialDiffuse.rgb * baseColor;
    
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = max(dot(normal, halfDir), 0.0);
    
    float specPower = spec;
    for(int i = 1; i < MaterialShine; ++i) {
        specPower *= spec;
    }
    
    vec3 specular = specPower * light.specular.rgb * MaterialSpecular.rgb;
    
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 baseColor = texture(myTexture, TextureCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(VecToEye);
    
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        if (lights[i].enabled) {
            result += calculateLight(lights[i], normal, viewDir, baseColor);
        }
    }
    
    FragColor = vec4(result, 1.0);
}