#version 330 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube environmentMap;

void main() {
    vec3 color = texture(environmentMap, localPos).rgb;

    // Match pbr.frag's tonemap/gamma so the sky and lit objects agree visually.
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
