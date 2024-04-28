//
// Created by Mohamad on 28/04/2024.
//
#include "game_file_reader.h"

std::string glcpp::utils::read_file(const std::string &filename) {

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return buffer.str();

}

