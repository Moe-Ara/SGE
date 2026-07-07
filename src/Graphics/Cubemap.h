#ifndef SGE_GRAPHICS_CUBEMAP_H
#define SGE_GRAPHICS_CUBEMAP_H

#include <glad/gl.h>

namespace SGE::GRAPHICS {

    // A minimal GL_TEXTURE_CUBE_MAP wrapper for offscreen-rendered (not
    // file-loaded) environment maps: the raw captured sky, its diffuse
    // irradiance convolution, and its roughness-prefiltered specular mips.
    class Cubemap {
    public:
        // internalFormat/format/type e.g. GL_RGB16F / GL_RGB / GL_FLOAT for HDR.
        Cubemap(int faceSize, GLint internalFormat, GLenum format, GLenum type, bool withMips);
        ~Cubemap();

        Cubemap(const Cubemap&) = delete;
        Cubemap& operator=(const Cubemap&) = delete;

        void bind(int unit) const;
        void generateMipmaps() const;

        GLuint getId() const { return id; }
        int getFaceSize() const { return faceSize; }

    private:
        GLuint id{0};
        int faceSize;
    };

} // namespace SGE::GRAPHICS

#endif // SGE_GRAPHICS_CUBEMAP_H
