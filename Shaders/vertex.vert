#version 330 core

layout (location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

out vec3 fragColor;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 uniformColor;
uniform mat3 normalMatrix;

void main() {
    // Calculate fragment position in world space
    FragPos = vec3(transform * vec4(position, 1.0));

    // Calculate normal in world space
    Normal = normalize(normalMatrix * normal);

    // Transform vertex position
    gl_Position = projection * view * transform * vec4(position, 1.0);

    // Pass color to fragment shader
    fragColor = uniformColor;
}
