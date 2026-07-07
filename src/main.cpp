#include "Core/Application.h"
#include "Exceptions/SGE_Exception.h"
#include <exception>
#include <iostream>

int main() {
    try {
        SGE::CORE::Application application{};
        application.run();
    } catch (SGE::EXCEPTIONS::SGE_Exception& e) {
        std::cerr << "SGE_Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "UnknownError" << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
