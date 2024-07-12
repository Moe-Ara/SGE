//
// Created by Mohamad on 12/07/2024.
//

#include "SGE_Exception.h"

namespace SGE {
    namespace EXCEPTIONS {
        SGE_Exception::SGE_Exception(const char *const message) throw(): runtime_error(message), m_message(message){

        }

        char const *SGE_Exception::what() const throw() {
            return ("SGE_ERROR:"+m_message).c_str();
        }
    } // EXCEPTIONS
} // SGE