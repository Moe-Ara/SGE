#ifndef SGE_GRAPHICS_ENVIRONMENT_H
#define SGE_GRAPHICS_ENVIRONMENT_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <memory>
#include "Cubemap.h"

namespace SGE::GRAPHICS {

    // Bakes a procedural sky into a cubemap, then convolves it into image-based
    // lighting data: diffuse irradiance (for the PBR ambient diffuse term) and a
    // roughness-prefiltered specular cubemap + split-sum BRDF LUT (for the PBR
    // ambient specular term). This is what the engine means by "global
    // illumination" — static, image-based ambient lighting derived from the
    // sky, not dynamic multi-bounce light transport. Baking happens once, here,
    // at construction; nothing in this class runs per frame.
    class Environment {
    public:
        Environment();
        ~Environment();

        Environment(const Environment&) = delete;
        Environment& operator=(const Environment&) = delete;

        GLuint getEnvironmentCubemapId() const;
        GLuint getIrradianceMapId() const;
        GLuint getPrefilterMapId() const;
        GLuint getBRDFLUTId() const { return brdfLUT; }
        float getPrefilterMaxLod() const { return static_cast<float>(kPrefilterMipLevels - 1); }
        GLuint getCubeVAO() const { return cubeVAO; }

    private:
        static constexpr int kEnvironmentFaceSize = 256;
        static constexpr int kIrradianceFaceSize = 32;
        static constexpr int kPrefilterBaseFaceSize = 128;
        static constexpr int kPrefilterMipLevels = 5;
        static constexpr int kBRDFLUTSize = 256;

        std::unique_ptr<Cubemap> environmentCubemap;
        std::unique_ptr<Cubemap> irradianceMap;
        std::unique_ptr<Cubemap> prefilterMap;
        GLuint brdfLUT{0};

        GLuint cubeVAO{0};
        GLuint cubeVBO{0};

        void setupCubeMesh();
        void captureSky(GLuint captureFBO, GLuint captureRBO);
        void convolveIrradiance(GLuint captureFBO, GLuint captureRBO);
        void prefilterSpecular(GLuint captureFBO, GLuint captureRBO);
        void generateBRDFLUT(GLuint captureFBO, GLuint captureRBO);
    };

} // namespace SGE::GRAPHICS

#endif // SGE_GRAPHICS_ENVIRONMENT_H
