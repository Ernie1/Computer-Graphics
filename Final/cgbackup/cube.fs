#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace, float diff)
{
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;
   float closestDepth = texture(shadowMap, projCoords.xy).r;
   float currentDepth = projCoords.z;
   float bias = max(0.05 * (1.0 - diff), 0.005);
   float shadow = 0.0;
   vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
   for(int x = -1; x <= 1; ++x)
   {
       for(int y = -1; y <= 1; ++y)
       {
           float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
           shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
       }
   }
   shadow /= 9.0;
   if(projCoords.z > 1.0)
       shadow = 0.0;
   return shadow;
}

void main()
{
   float ambientStrength = 0.2;
   float diffuseStrength = 1.0;
   float specularStrength = 1.0;
   float shininess = 32.0;
   vec3 ambient = ambientStrength * lightColor;
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diffuseStrength * diff * lightColor;
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
   vec3 specular = specularStrength * spec * lightColor;
   float shadow = min(shadowCalculation(FragPosLightSpace, diff), 0.75);
   FragColor = vec4((ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor, 1.0);
}
