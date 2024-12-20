#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;    
uniform vec3 lightColor;  
uniform vec3 viewPos;     
uniform sampler2D diffuseTexture;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
    // vec3 objectColor = texture(diffuseTexture, TexCoords).rgb; 
    // float ambientStrength = 0.5;
    // vec3 ambient = ambientStrength * lightColor;

    // vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(lightPos - FragPos); 
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = diff * lightColor;

    // float specularStrength = 0.5;
    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // vec3 specular = specularStrength * spec * lightColor;
   
    // vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(objectColor, 1.0);
}
