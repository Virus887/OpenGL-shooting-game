#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;  
uniform Material material;

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;
  
uniform vec3 viewPos; 

uniform sampler2D texture1;

void main()
{    
    // ambient
    vec3 ambient = light.ambient  * vec3(texture(material.diffuse, TexCoords.xy));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords.xy));  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords.xy));
        
    vec3 result = (ambient + diffuse + specular) * vec3(texture(texture1, TexCoords.xy));
    FragColor = vec4(result, 1.0);
}