//
// Created by Mohamad on 12/07/2024.
//

#include "SGE_Exception.h"

namespace SGE::EXCEPTIONS {

SGE_Exception::SGE_Exception(const char* message)
    : std::runtime_error(message ? message : ""), m_message(message ? message : "") {}

const char* SGE_Exception::what() const noexcept {
    return m_message.c_str();
}

} // namespace SGE::EXCEPTIONS