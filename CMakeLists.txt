cmake_minimum_required(VERSION 3.28)
project(GameEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(libs)

find_package(Vulkan REQUIRED)

add_library(engine
        src/engine/engine.cpp
        src/engine/engine.hpp
        src/engine/window.cpp
        src/engine/window.hpp
        src/engine/vulkan_context.cpp
        src/engine/vulkan_context.hpp)
target_include_directories(engine PUBLIC src/)
target_link_libraries(engine PUBLIC spdlog::spdlog Vulkan::Vulkan glfw glm::glm)
target_compile_definitions(engine PUBLIC -DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

add_library(engine::engine ALIAS engine)

add_subdirectory(example)
