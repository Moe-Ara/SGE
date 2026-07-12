#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <string>
#include <glm/glm.hpp>

namespace SGE::GRAPHICS {
    class Shader {
    private:
        std::string vert_shader_path;
        std::string frag_shader_path;
        GLuint shader{0};

    public:
        Shader(std::string vertexPath, std::string fragmentPath);
        ~Shader();

        void use();
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setFloat(const std::string &name, float value) const;
        void setInt(const std::string &name, int value) const;

        void enable() const;
        void disable();

        // PBR specific uniforms
        void setPBRMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao);
        void setPBRLight(const std::string& lightName,
                         const glm::vec3& position,
                         const glm::vec3& color,
                         float intensity);

        // Shadow specific uniforms
        void setLightSpaceMatrix(const glm::mat4& lightSpaceMatrix);
        void setLightPos(const glm::vec3& lightPos);

    private:
        GLuint load();
        bool compileShader(GLuint shader, const char* code);
        GLint getUniformLocation(const GLchar *name) const;
    public:
        void setUniformFloat1(const GLchar *name, float value) const;
        void setUniformInt1(const GLchar *name, int value) const;
        void setUniformFloat2(const GLchar *name, glm::vec2 vector2) const;
        void setUniformFloat3(const GLchar *name, glm::vec3 vector3) const;
        void setUniformFloat4(const GLchar *name, glm::vec4 vector4) const;
        void setUniformMat4(const GLchar *name, const glm::mat4& matrix) const;
        void setUniformMat3(const GLchar *name, const glm::mat3 &matrix) const;
    };
}

#endif

