#include "Core/Application.h"
#include "Exceptions/SGE_Exception.h"
#include "Core/AssetLocator.h"
#include <exception>
#include <iostream>

int main(int argc, char** argv) {
    try {
        std::cerr << "[SGE] Locating resources...\n";
        SGE::CORE::AssetLocator::initialize(argc > 0 ? argv[0] : "");
        std::cerr << "[SGE] Resources: " << SGE::CORE::AssetLocator::root() << '\n';
        std::cerr << "[SGE] Initializing engine...\n";
        SGE::CORE::Application application{};
        std::cerr << "[SGE] Engine initialized; entering main loop.\n";
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
