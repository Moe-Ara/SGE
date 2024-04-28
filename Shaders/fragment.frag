#version 330 core

in vec3 fragColor; // Input fragment color

out vec4 FragColor; // Output fragment color

void main() {
    FragColor = vec4(fragColor, 1.0); // Output fragment color (with alpha = 1.0)
}