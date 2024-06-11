#include <vector>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include "sge_shader.h"
#include <iostream>

namespace SGE::graphics {

    // Constructor
    sge_shader::sge_shader(const char* vertPath, const char* fragPath)
            : vert_shader_path(vertPath), frag_shader_path(fragPath) {
        shader = load();
    }

    sge_shader::~sge_shader() {
        glDeleteProgram(shader);
    }

    void sge_shader::enable() const {
        glUseProgram(shader);
    }

    void sge_shader::disable() {
        glUseProgram(0);
    }

    GLuint sge_shader::load() {
        auto program = glCreateProgram();
        auto vertex = glCreateShader(GL_VERTEX_SHADER);
        auto fragment = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vertCodeStr = utils::read_file(vert_shader_path.c_str());
        std::string fragCodeStr = utils::read_file(frag_shader_path.c_str());

        const char* vertCode = vertCodeStr.c_str();
        const char* fragCode = fragCodeStr.c_str();

        if (!check_compilation_errors(vertex, vertCode)) {
            glDeleteShader(vertex);
            return 0;
        }

        if (!check_compilation_errors(fragment, fragCode)) {
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
            std::cerr << error.data() << std::endl;
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

    bool sge_shader::check_compilation_errors(GLuint shader, const char* code) {
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

    void sge_shader::setUniformFloat1(const GLchar *name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void sge_shader::setUniformInt1(const GLchar *name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void sge_shader::setUniformFloat2(const GLchar *name, glm::vec2 vector2) {
        glUniform2f(getUniformLocation(name), vector2.x, vector2.y);
    }

    void sge_shader::setUniformFloat3(const GLchar *name, glm::vec3 vector3) {
        glUniform3f(getUniformLocation(name), vector3.x, vector3.y, vector3.z);
    }

    void sge_shader::setUniformFloat4(const GLchar *name, glm::vec4 vector4) {
        glUniform4f(getUniformLocation(name), vector4.x, vector4.y, vector4.z, vector4.w);
    }

    void sge_shader::setUniformMat4(const GLchar *name, const glm::mat4& matrix) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    GLint sge_shader::getUniformLocation(const GLchar *name) {
        return glGetUniformLocation(this->shader, name);
    }
}
