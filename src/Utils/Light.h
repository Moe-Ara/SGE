#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    
    // Attenuation for point lights
    float constant;
    float linear;
    float quadratic;
    
    // Spot light properties
    float cutOff;
    float outerCutOff;
    
    // Shadow properties
    bool castsShadows;
    
    Light() : 
        position(0.0f, 0.0f, 0.0f),
        color(1.0f, 1.0f, 1.0f),
        intensity(1.0f),
        constant(1.0f),
        linear(0.09f),
        quadratic(0.032f),
        cutOff(0.0f),
        outerCutOff(0.0f),
        castsShadows(false) {}
};

struct DirectionalLight : public Light {
    glm::vec3 direction;
    
    DirectionalLight() : Light(), direction(0.0f, -1.0f, 0.0f) {}
};

struct PointLight : public Light {
    PointLight() : Light() {
        constant = 1.0f;
        linear = 0.09f;
        quadratic = 0.032f;
    }
};

struct SpotLight : public Light {
    glm::vec3 direction;
    
    SpotLight() : Light() {
        cutOff = glm::cos(glm::radians(12.5f));
        outerCutOff = glm::cos(glm::radians(17.5f));
        direction = glm::vec3(0.0f, -1.0f, 0.0f);
    }
};

#endif // LIGHT_H
