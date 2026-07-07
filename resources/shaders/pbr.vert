#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUV;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    FragPos = vec3(transform * vec4(aPosition, 1.0));
    Normal = normalize(normalMatrix * aNormal);
    UV = aUV;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
