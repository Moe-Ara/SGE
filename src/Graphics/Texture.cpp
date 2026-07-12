#include "Texture.h"
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <stdexcept>

namespace SGE::GRAPHICS {

    std::shared_ptr<Texture> Texture::loadFromFile(const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) {
            std::cerr << "Error loading texture: " << path << std::endl;
            return nullptr;
        }

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 2) format = GL_RG;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;
        else {
            stbi_image_free(data);
            throw std::runtime_error("Unsupported texture channel count for: " + path);
        }

        auto texture = std::shared_ptr<Texture>(new Texture());
        texture->width = width;
        texture->height = height;

        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        GLint previousUnpackAlignment = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        return texture;
    }

    std::shared_ptr<Texture> Texture::createCheckerboard(int size, const glm::vec3& colorA, const glm::vec3& colorB, int tiles) {
        if (size <= 0) {
            throw std::invalid_argument("Checkerboard size must be positive");
        }
        tiles = glm::clamp(tiles, 1, size);
        std::vector<unsigned char> pixels(static_cast<size_t>(size) * size * 3);
        const int tileSize = std::max(size / tiles, 1);

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                const bool isA = ((x / tileSize) + (y / tileSize)) % 2 == 0;
                const glm::vec3& c = isA ? colorA : colorB;
                const size_t idx = (static_cast<size_t>(y) * size + x) * 3;
                pixels[idx + 0] = static_cast<unsigned char>(glm::clamp(c.r, 0.0f, 1.0f) * 255.0f);
                pixels[idx + 1] = static_cast<unsigned char>(glm::clamp(c.g, 0.0f, 1.0f) * 255.0f);
                pixels[idx + 2] = static_cast<unsigned char>(glm::clamp(c.b, 0.0f, 1.0f) * 255.0f);
            }
        }

        auto texture = std::shared_ptr<Texture>(new Texture());
        texture->width = size;
        texture->height = size;

        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        GLint previousUnpackAlignment = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    Texture::~Texture() {
        if (id != 0) {
            glDeleteTextures(1, &id);
        }
    }

    void Texture::bind(int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

} // namespace SGE::GRAPHICS
