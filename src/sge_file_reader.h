//
// Created by Mohamad on 28/04/2024.
//

#ifndef GLCPP_SGE_FILE_READER_H
#define GLCPP_SGE_FILE_READER_H


#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace SGE::utils{

    std::string read_file(const std::string& filename);

}
#endif //GLCPP_SGE_FILE_READER_H
