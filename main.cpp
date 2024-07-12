#include "src/Core/Application.h"

int main() {

    try {
        SGE::CORE::Application application{};
        application.run();
    } catch (SGE::EXCEPTIONS::SGE_Exception &e) {
        e.what();
    }
    catch (...) {
        std::cerr << "UnknownError" << std::endl;
    }

    return 0;
}
