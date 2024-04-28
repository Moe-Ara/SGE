#version 330 core

layout (location = 0) in vec3 position; // Input vertex position

out vec3 fragColor; // Output fragment color

uniform mat4 model=mat4(1.0); // Model matrix
uniform mat4 view=mat4(1.0); // View matrix
uniform mat4 projection=mat4(1.0); // Projection matrix
uniform vec3 color=vec3(0.5, 0.0, 1.0);
void main() {
    gl_Position = projection*model*view*vec4(position, 1.0); // Transform vertex position
    fragColor =color; // Set fragment color (example: magenta)
}