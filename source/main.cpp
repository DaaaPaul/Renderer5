#include "vulkan/Backend.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Vulkan::Backend backend{};
    } catch(std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
    }

    return 0;
}