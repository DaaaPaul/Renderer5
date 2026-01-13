#include "vulkan/Backend.h"
#include "vulkan/Queues.h"
#include "vulkan/Swapchain.h"
#include "vulkan/Memory.h"
#include <iostream>
#include <stdexcept>
#include <utility>

int main() {
    try {
        Vulkan::Backend backend{};
        Vulkan::Queues queues(std::move(backend));
        Vulkan::Swapchain swapchain(std::move(queues));
        Vulkan::Memory memory(std::move(swapchain));
    } catch(std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
    }

    return 0;
}