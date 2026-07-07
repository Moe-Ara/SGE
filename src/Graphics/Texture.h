#ifndef SGE_GRAPHICS_TEXTURE_H
#define SGE_GRAPHICS_TEXTURE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace SGE::GRAPHICS {

    // A minimal GL_TEXTURE_2D wrapper. Textures are always stored in linear (non-sRGB)
    // internal formats; shaders that treat a sampled texture as sRGB-encoded albedo are
    // expected to gamma-decode it themselves (see resources/shaders/pbr.frag).
    class Texture {
    public:
        static std::shared_ptr<Texture> loadFromFile(const std::string& path);
        static std::shared_ptr<Texture> createCheckerboard(int size, const glm::vec3& colorA, const glm::vec3& colorB, int tiles = 8);

        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void bind(int unit) const;

        GLuint getId() const { return id; }

    private:
        Texture() = default;

        GLuint id{0};
        int width{0};
        int height{0};
    };

} // namespace SGE::GRAPHICS

#endif // SGE_GRAPHICS_TEXTURE_H
