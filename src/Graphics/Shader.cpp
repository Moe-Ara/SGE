#include <vector>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "../Utils/file_reader.h"
#include <iostream>

namespace SGE::graphics {

    // Constructor
    Shader::Shader(const char* vertPath, const char* fragPath)
            : vert_shader_path(vertPath), frag_shader_path(fragPath) {
        shader = load();
    }

    Shader::~Shader() {
        glDeleteProgram(shader);
    }

    void Shader::enable() const {
        glUseProgram(shader);
    }

    void Shader::disable() {
        glUseProgram(0);
    }

    GLuint Shader::load() {
        auto program = glCreateProgram();
        auto vertex = glCreateShader(GL_VERTEX_SHADER);
        auto fragment = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vertCodeStr = utils::read_file(vert_shader_path.c_str());
        std::string fragCodeStr = utils::read_file(frag_shader_path.c_str());

        const char* vertCode = vertCodeStr.c_str();
        const char* fragCode = fragCodeStr.c_str();

        if (!compileShader(vertex, vertCode)) {
            glDeleteShader(vertex);
            return 0;
        }

        if (!compileShader(fragment, fragCode)) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return 0;
        }

        // Attach shaders to the program
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);

        // Link the program
        glLinkProgram(program);

        // Check linking errors
        GLint result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result == GL_FALSE) {
            GLint length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> error(length);
            glGetProgramInfoLog(program, length, &length, error.data());
            std::cerr << "Shader linking error: " << error.data() << std::endl;
            glDeleteProgram(program);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return 0;
        }

        // Clean up shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return program;
    }

    bool Shader::compileShader(GLuint shader, const char* code) {
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        // Check compilation errors
        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> error(length);
            glGetShaderInfoLog(shader, length, &length, error.data());
            std::cerr << error.data() << std::endl;
            return false;
        }

        return true;
    }

    void Shader::setUniformFloat1(const GLchar *name, float value) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void Shader::setUniformInt1(const GLchar *name, int value) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value);
        }
    }

    void Shader::setUniformFloat2(const GLchar *name, glm::vec2 vector2) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform2f(location, vector2.x, vector2.y);
        }
    }

    void Shader::setUniformFloat3(const GLchar *name, glm::vec3 vector3) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform3f(location, vector3.x, vector3.y, vector3.z);
        }
    }

    void Shader::setUniformFloat4(const GLchar *name, glm::vec4 vector4) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniform4f(location, vector4.x, vector4.y, vector4.z, vector4.w);
        }
    }

    void Shader::setUniformMat4(const GLchar *name, const glm::mat4& matrix) {
        enable();  // Ensure shader is active
        GLint location = getUniformLocation(name);
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
    void Shader::setUniformMat3(const GLchar *name, const glm::mat3 &matrix) {
        enable();
        GLint location = getUniformLocation(name);
        if (location == -1) {
            std::cerr << "Warning: uniform '" << name << "' doesn't exist in the shader!" << std::endl;
        } else {
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }

    GLint Shader::getUniformLocation(const GLchar *name) {
        GLint location = glGetUniformLocation(this->shader, name);
        if (location == -1) {
            std::cerr << "Warning: uniform '" << name << "' doesn't exist in the shader!" << std::endl;
        }
        return location;
    }

    bool Shader::check_compilation_errors(GLuint shader, const char *code) {
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        // Check compilation errors
        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> error(length);
            glGetShaderInfoLog(shader, length, &length, error.data());
            std::cerr << error.data() << std::endl;
            return false;
        }

        return true;
    }


}
