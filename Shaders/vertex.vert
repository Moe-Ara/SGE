#version 330 core

layout (location = 0) in vec3 position; // Input vertex position
//layout(location=1) in vec3 color;
//layout(location=2) in vec3 normal;
//layout(location=3) in vec2 uv;

out vec3 fragColor; // Output fragment color


uniform mat4 transform =mat4(1.0); // Model matrix
uniform mat4 view=mat4(1.0); // View matrix
uniform mat4 projection=mat4(1.0); // Projection matrix
uniform vec3 color=vec3(0.5, 0.0, 1.0);
uniform mat4 normalMatrix;
void main() {
    gl_Position = projection* view*transform *vec4(position, 1.0); // Transform vertex position
//    vec3 normalWorldSpace=normalize(mat3(normalMatrix)*normal);
//    float lightIntensity=AMBIENT+max(dot(normalWorldSpace, Direction_to_light),0);
    fragColor=/*lightIntensity*/color;
}