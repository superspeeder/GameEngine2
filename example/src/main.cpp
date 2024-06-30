#include <engine/engine.hpp>
#include <engine/vulkan_context.hpp>
#include <engine/window.hpp>

#include <iostream>

void run() {
    auto vulkan_context = engine::VulkanContext::create({});
    auto window = engine::Window::create({}, vulkan_context);

    while (!window->shouldClose()) {
        engine::pollEvents();
    }
}

int main() {
    glfwInit();
    try {
        run();
    } catch (std::exception& e) {
        spdlog::critical("Error: {}", e.what());
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}