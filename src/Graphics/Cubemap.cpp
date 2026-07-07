#include "Cubemap.h"

namespace SGE::GRAPHICS {

    Cubemap::Cubemap(int faceSize, GLint internalFormat, GLenum format, GLenum type, bool withMips)
        : faceSize(faceSize) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        for (int face = 0; face < 6; ++face) {
            glTexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face), 0,
                         internalFormat, faceSize, faceSize, 0, format, type, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                         withMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

        if (withMips) {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    Cubemap::~Cubemap() {
        if (id != 0) {
            glDeleteTextures(1, &id);
        }
    }

    void Cubemap::bind(int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    void Cubemap::generateMipmaps() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

} // namespace SGE::GRAPHICS
