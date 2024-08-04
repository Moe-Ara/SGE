//
// Created by Mohamad on 12/07/2024.
//

#ifndef GLCPP_SGE_EXCEPTION_H
#define GLCPP_SGE_EXCEPTION_H

#include <stdexcept>


    namespace SGE::EXCEPTIONS {

        class SGE_Exception : public std::runtime_error{
        public:
            SGE_Exception(char const* message) throw();
            virtual char const* what() const throw();
            ~SGE_Exception() override{};
        private:
            std::string m_message;
        };

    } // EXCEPTIONS
// SGE

#endif //GLCPP_SGE_EXCEPTION_H
