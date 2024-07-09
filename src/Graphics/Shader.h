#ifndef SGE_GRAPHICS_SHADER_H
#define SGE_GRAPHICS_SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

namespace SGE::graphics {

    class Shader {
    public:
        // Constructor and Destructor
        Shader(const char* vertPath, const char* fragPath);
        ~Shader();

        // Enable and disable the shader program
        void enable() const;
        void disable();

        // Set uniform variables
        void setUniformFloat1(const GLchar* name, float value);
        void setUniformInt1(const GLchar* name, int value);
        void setUniformFloat2(const GLchar* name, glm::vec2 vector2);
        void setUniformFloat3(const GLchar* name, glm::vec3 vector3);
        void setUniformFloat4(const GLchar* name, glm::vec4 vector4);
        void setUniformMat4(const GLchar* name, const glm::mat4& matrix);
        void setUniformMat3(const GLchar* name, const glm::mat3& matrix);


    private:
        // Shader program ID
        GLuint shader;
        // File paths for the vertex and fragment shaders
        std::string vert_shader_path;
        std::string frag_shader_path;

        // Load and compile shaders, and link the shader program
        GLuint load();
        bool compileShader(GLuint shader, const char* code);
        GLint getUniformLocation(const GLchar* name);

        // Check for compilation errors
        bool check_compilation_errors(GLuint shader, const char* code);
    };
}

#endif // SGE_GRAPHICS_SHADER_H
