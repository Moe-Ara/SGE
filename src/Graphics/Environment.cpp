#include "Environment.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <iostream>

namespace SGE::GRAPHICS {

    namespace {
        void checkFramebufferComplete(const char* stage) {
            const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Environment bake: framebuffer incomplete during '" << stage
                          << "' (status 0x" << std::hex << status << std::dec
                          << "); IBL textures for this stage will be garbage." << std::endl;
            }
        }
        // Positions only, viewed from inside the cube (camera at the origin).
        // Face culling is disabled while drawing it, so winding order doesn't matter.
        constexpr float kCubeVertices[] = {
            -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
        };

        std::array<glm::mat4, 6> captureViews() {
            return {
                glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            };
        }

        const glm::mat4 kCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    }

    Environment::Environment() {
        setupCubeMesh();

        GLuint captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        captureSky(captureFBO, captureRBO);
        convolveIrradiance(captureFBO, captureRBO);
        prefilterSpecular(captureFBO, captureRBO);
        generateBRDFLUT(captureFBO, captureRBO);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteRenderbuffers(1, &captureRBO);
        glDeleteFramebuffers(1, &captureFBO);
    }

    Environment::~Environment() {
        if (cubeVBO != 0) glDeleteBuffers(1, &cubeVBO);
        if (cubeVAO != 0) glDeleteVertexArrays(1, &cubeVAO);
        if (brdfLUT != 0) glDeleteTextures(1, &brdfLUT);
    }

    void Environment::setupCubeMesh() {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void Environment::captureSky(GLuint captureFBO, GLuint captureRBO) {
        environmentCubemap = std::make_unique<Cubemap>(kEnvironmentFaceSize, GL_RGB16F, GL_RGB, GL_FLOAT, true);

        Shader shader("resources/shaders/cubemap_capture.vert", "resources/shaders/sky_capture.frag");
        shader.use();
        shader.setMat4("captureProjection", kCaptureProjection);
        shader.setVec3("sunDirection", glm::normalize(glm::vec3(0.35f, 0.55f, 0.4f)));
        shader.setVec3("zenithColor", glm::vec3(0.10f, 0.28f, 0.62f));
        shader.setVec3("horizonColor", glm::vec3(0.65f, 0.75f, 0.85f));
        shader.setVec3("groundColor", glm::vec3(0.08f, 0.08f, 0.09f));
        shader.setFloat("sunIntensity", 6.0f);
        shader.setFloat("sunSize", 0.9995f);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kEnvironmentFaceSize, kEnvironmentFaceSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        glViewport(0, 0, kEnvironmentFaceSize, kEnvironmentFaceSize);
        const auto views = captureViews();
        glBindVertexArray(cubeVAO);
        for (int face = 0; face < 6; ++face) {
            shader.setMat4("captureView", views[face]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),
                                    environmentCubemap->getId(), 0);
            if (face == 0) checkFramebufferComplete("captureSky");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        environmentCubemap->generateMipmaps();
    }

    void Environment::convolveIrradiance(GLuint captureFBO, GLuint captureRBO) {
        irradianceMap = std::make_unique<Cubemap>(kIrradianceFaceSize, GL_RGB16F, GL_RGB, GL_FLOAT, false);

        Shader shader("resources/shaders/cubemap_capture.vert", "resources/shaders/irradiance_convolve.frag");
        shader.use();
        shader.setMat4("captureProjection", kCaptureProjection);
        shader.setInt("environmentMap", 0);
        environmentCubemap->bind(0);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kIrradianceFaceSize, kIrradianceFaceSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        glViewport(0, 0, kIrradianceFaceSize, kIrradianceFaceSize);
        const auto views = captureViews();
        glBindVertexArray(cubeVAO);
        for (int face = 0; face < 6; ++face) {
            shader.setMat4("captureView", views[face]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),
                                    irradianceMap->getId(), 0);
            if (face == 0) checkFramebufferComplete("convolveIrradiance");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
    }

    void Environment::prefilterSpecular(GLuint captureFBO, GLuint captureRBO) {
        prefilterMap = std::make_unique<Cubemap>(kPrefilterBaseFaceSize, GL_RGB16F, GL_RGB, GL_FLOAT, true);

        Shader shader("resources/shaders/cubemap_capture.vert", "resources/shaders/prefilter_convolve.frag");
        shader.use();
        shader.setMat4("captureProjection", kCaptureProjection);
        shader.setInt("environmentMap", 0);
        environmentCubemap->bind(0);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        const auto views = captureViews();
        glBindVertexArray(cubeVAO);

        for (int mip = 0; mip < kPrefilterMipLevels; ++mip) {
            const int mipSize = kPrefilterBaseFaceSize >> mip;
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
            glViewport(0, 0, mipSize, mipSize);

            const float roughness = static_cast<float>(mip) / static_cast<float>(kPrefilterMipLevels - 1);
            shader.setFloat("roughness", roughness);

            for (int face = 0; face < 6; ++face) {
                shader.setMat4("captureView", views[face]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),
                                        prefilterMap->getId(), mip);
                if (face == 0) checkFramebufferComplete("prefilterSpecular");
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        glBindVertexArray(0);
    }

    void Environment::generateBRDFLUT(GLuint captureFBO, GLuint captureRBO) {
        glGenTextures(1, &brdfLUT);
        glBindTexture(GL_TEXTURE_2D, brdfLUT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, kBRDFLUTSize, kBRDFLUTSize, 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kBRDFLUTSize, kBRDFLUTSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);
        checkFramebufferComplete("generateBRDFLUT");

        glViewport(0, 0, kBRDFLUTSize, kBRDFLUTSize);

        GLuint quadVAO, quadVBO;
        const float quadVertices[] = {
            // positions   // uv
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        Shader shader("resources/shaders/brdf_lut.vert", "resources/shaders/brdf_lut.frag");
        shader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glDeleteBuffers(1, &quadVBO);
        glDeleteVertexArrays(1, &quadVAO);
    }

    GLuint Environment::getEnvironmentCubemapId() const { return environmentCubemap->getId(); }
    GLuint Environment::getIrradianceMapId() const { return irradianceMap->getId(); }
    GLuint Environment::getPrefilterMapId() const { return prefilterMap->getId(); }

} // namespace SGE::GRAPHICS
