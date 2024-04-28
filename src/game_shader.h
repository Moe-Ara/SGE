//
// Created by Mohamad on 28/04/2024.
//

#ifndef GLCPP_GAME_SHADER_H
#define GLCPP_GAME_SHADER_H
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <memory>
#include "game_file_reader.h"
namespace glcpp::graphics{
    class game_shader {
    public:
        game_shader(const char* vert_shader_path,const char* frag_shader_path);
        ~game_shader();
        void enable() const;
        static void disable();
        void setUniformFloat1(const GLchar* name, float value);
        void setUniformInt1(const GLchar* name, int value);
        void setUniformFloat2(const GLchar* name, glm::vec2 vector2);
        void setUniformFloat3(const GLchar* name, glm::vec3 vector3);
        void setUniformFloat4(const GLchar* name, glm::vec4 vector4);
        void setUniformMat4(const GLchar* name, const glm::mat4& matrix);
    private:
        GLuint shader;
        std::unique_ptr<char[]> vert_shader_path;
        std::unique_ptr<char[]> frag_shader_path;
        GLuint load();
        bool check_compilation_errors(GLuint shader, const char* code);
        GLint getUniformLocation(const GLchar* name);

    };

}


#endif //GLCPP_GAME_SHADER_H
