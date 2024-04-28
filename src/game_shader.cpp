//
// Created by Mohamad on 28/04/2024.
//

#include <vector>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include "game_shader.h"

// Constructor
glcpp::graphics::game_shader::game_shader(const char* vertPath, const char* fragPath)
        : vert_shader_path(std::make_unique<char[]>(std::strlen(vertPath) + 1)),
          frag_shader_path(std::make_unique<char[]>(std::strlen(fragPath) + 1)) {
    std::strcpy(vert_shader_path.get(), vertPath);
    std::strcpy(frag_shader_path.get(), fragPath);
    shader=load();
}
glcpp::graphics::game_shader::~game_shader() {

    glDeleteProgram(shader);

}

void glcpp::graphics::game_shader::enable() const {
glUseProgram(shader);
}

void glcpp::graphics::game_shader::disable() {
    glUseProgram(0);

}

GLuint glcpp::graphics::game_shader::load() {
    auto program = glCreateProgram();
    auto vertex = glCreateShader(GL_VERTEX_SHADER);
    auto fragment = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertCodeStr=utils::read_file(vert_shader_path.get());
    std::string fragCodeStr=utils::read_file(frag_shader_path.get());

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
        glGetProgramInfoLog(program, length, &length, &error[0]);
        std::cerr << &error[0] << std::endl;
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

bool glcpp::graphics::game_shader::check_compilation_errors(GLuint shader, const char* code) {
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    // Check compilation errors
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> error(length);
        glGetShaderInfoLog(shader, length, &length, &error[0]);
        std::cerr << &error[0] << std::endl;
        return false;
    }

    return true;
}

void glcpp::graphics::game_shader::setUniformFloat1(const GLchar *name, float value) {
    glUniform1f(getUniformLocation(name),value);

}

void glcpp::graphics::game_shader::setUniformInt1(const GLchar *name, int value) {
    glUniform1i(getUniformLocation(name),value);

}

void glcpp::graphics::game_shader::setUniformFloat2(const GLchar *name, glm::vec2 vector2) {
    glUniform2f(getUniformLocation(name),vector2.x,vector2.y);

}

void glcpp::graphics::game_shader::setUniformFloat3(const GLchar *name, glm::vec3 vector3) {
    glUniform3f(getUniformLocation(name),vector3.x,vector3.y,vector3.z);

}

void glcpp::graphics::game_shader::setUniformFloat4(const GLchar *name, glm::vec4 vector4) {
    glUniform4f(getUniformLocation(name),vector4.x,vector4.y,vector4.z,vector4.w);

}

void glcpp::graphics::game_shader::setUniformMat4(const GLchar *name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name),1,GL_FALSE,glm::value_ptr(matrix));
}

GLint glcpp::graphics::game_shader::getUniformLocation(const GLchar *name) {
    glGetUniformLocation(this->shader,name);
}

