#include "vulkan/Backend.h"
#include "vulkan/Queues.h"
#include <iostream>
#include <stdexcept>
#include <utility>

int main() {
    try {
        Vulkan::Backend backend{};
        Vulkan::Queues queues(std::move(backend));
    } catch(std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
    }

    return 0;
}