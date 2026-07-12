//
// Created by Mohamad on 28/04/2024.
//
#include "file_reader.h"
#include <stdexcept>

std::string SGE::UTILS::read_file(const std::string &filename) {

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return buffer.str();

}

