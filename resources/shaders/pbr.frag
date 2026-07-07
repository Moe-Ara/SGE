#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

out vec4 FragColor;

uniform vec3 viewPos;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform sampler2D albedoMap;
uniform bool useAlbedoMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform float prefilterMaxLod;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 4
uniform PointLight lights[MAX_LIGHTS];
uniform int numLights;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;
    return NdotV / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Roughness-aware variant used for ambient/IBL Fresnel, so rough dielectrics
// don't get an unnaturally strong grazing-angle highlight from the sky.
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // Sampled albedo textures are authored in sRGB; decode to linear before lighting.
    vec3 baseAlbedo = useAlbedoMap ? pow(texture(albedoMap, UV).rgb, vec3(2.2)) : albedo;

    // Dielectrics use a flat 0.04 F0; metals tint F0 with albedo.
    vec3 F0 = mix(vec3(0.04), baseAlbedo, metallic);

    vec3 Lo = vec3(0.0);
    int count = min(numLights, MAX_LIGHTS);
    for (int i = 0; i < count; ++i) {
        vec3 L = normalize(lights[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / max(distance * distance, 0.0001);
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // Energy conservation: reflected light doesn't get diffused; metals have no diffuse term.
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * baseAlbedo / PI + specular) * radiance * NdotL;
    }

    // Image-based ambient lighting derived from the baked sky (see
    // src/Graphics/Environment.h): a diffuse term from the irradiance
    // convolution and a specular term from the roughness-prefiltered mips +
    // split-sum BRDF LUT, combined the same energy-conserving way as the
    // direct lights above.
    vec3 ambientF = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 ambientKS = ambientF;
    vec3 ambientKD = (vec3(1.0) - ambientKS) * (1.0 - metallic);

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * baseAlbedo;

    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * prefilterMaxLod).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (ambientF * envBRDF.x + envBRDF.y);

    vec3 ambient = (ambientKD * diffuseIBL + specularIBL) * ao;
    vec3 color = ambient + Lo;

    // Reinhard tonemap + gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
