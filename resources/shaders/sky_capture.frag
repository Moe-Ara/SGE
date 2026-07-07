#version 330 core

in vec3 localPos;
out vec4 FragColor;

uniform vec3 sunDirection; // normalized, points toward the sun
uniform vec3 zenithColor;
uniform vec3 horizonColor;
uniform vec3 groundColor;
uniform float sunIntensity;
uniform float sunSize; // cosine threshold for the sun disc

void main() {
    vec3 dir = normalize(localPos);

    vec3 sky;
    if (dir.y >= 0.0) {
        float t = pow(clamp(dir.y, 0.0, 1.0), 0.5);
        sky = mix(horizonColor, zenithColor, t);
    } else {
        float t = pow(clamp(-dir.y, 0.0, 1.0), 0.5);
        sky = mix(horizonColor, groundColor, t);
    }

    float cosAngle = dot(dir, normalize(sunDirection));
    float sunDisc = smoothstep(sunSize, sunSize + 0.001, cosAngle);
    float sunGlow = pow(max(cosAngle, 0.0), 64.0) * 0.5;

    vec3 color = sky + vec3(1.0, 0.95, 0.85) * (sunDisc * sunIntensity + sunGlow);

    FragColor = vec4(color, 1.0);
}
