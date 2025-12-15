#version 330 core
uniform vec4 EyePosition = vec4(0.0, 5.0, 0.0, 0.0);
uniform vec4 PrimaryColorVec4 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 LightAmbient = vec4(0.2, 0.2, 0.2, 1);
uniform vec4 LightDiffuse = vec4(1, 0.9, 0.8, 1);
uniform vec4 LightSpecular = vec4(0.9, 0.8, 0.7, 1);
uniform vec3 LightPosition = vec3(0, 8, 8);
uniform vec3 LightAttenuation = vec3(1, 0.1, 0.001);
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

void main()
{
    vec3 BaseColor = texture(myTexture, TextureCoord).rgb;
    
    // Start with ambient light
    FragColor = vec4(BaseColor * MaterialAmbient.rgb * LightAmbient.rgb, 1.0);
    
    vec3 LightDirection = normalize(LightPosition - VertexPosition);
    vec3 NormalizedNormal = normalize(Normal);
    
    float DifIntensity = max(dot(LightDirection, NormalizedNormal), 0.0);
    
    if (DifIntensity > 0.0)
    {
        float Distance = length(LightPosition - VertexPosition);
        float Attenuation = 1.0 / (LightAttenuation.x + LightAttenuation.y * Distance + LightAttenuation.z * Distance * Distance);
        
        vec3 DiffusePart = DifIntensity * LightDiffuse.rgb * MaterialDiffuse.rgb * BaseColor;
        FragColor.rgb += DiffusePart * Attenuation;
        
        vec3 VectorToEye = normalize(VecToEye);
        vec3 HalfVector = normalize(LightDirection + VectorToEye);
        
        float SpecIntensity = max(dot(HalfVector, NormalizedNormal), 0.0);
        
        float SpecPower = SpecIntensity;
        for(int i = 1; i < MaterialShine; ++i)
        {
            SpecPower *= SpecIntensity;
        }
        
        vec3 SpecularPart = SpecPower * LightSpecular.rgb * MaterialSpecular.rgb;
        FragColor.rgb += SpecularPart * Attenuation;  // Fixed: ADD instead of overwrite
    }
    
    //FragColor *= PrimaryColorVec4;
}