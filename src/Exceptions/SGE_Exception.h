//
// Created by Mohamad on 12/07/2024.
//

#ifndef GLCPP_SGE_EXCEPTION_H
#define GLCPP_SGE_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace SGE::EXCEPTIONS {

    class SGE_Exception : public std::runtime_error {
    public:
        explicit SGE_Exception(const char* message);
        explicit SGE_Exception(const std::string& message);
        const char* what() const noexcept override;

    private:
        std::string m_message;
    };

    } // EXCEPTIONS
// SGE

#endif //GLCPP_SGE_EXCEPTION_H
