#version 330 core

in vec3 fragColor;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Debug output to check the calculations
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * fragColor, 1.0);

    // Uncomment the following lines to output specific values for debugging
     FragColor = vec4(norm, 1.0); // Output the normal
     FragColor = vec4(lightDir, 1.0); // Output the light direction
     FragColor = vec4(diff, diff, diff, 1.0); // Output the diffuse component
     FragColor = vec4(spec, spec, spec, 1.0); // Output the specular component
}
