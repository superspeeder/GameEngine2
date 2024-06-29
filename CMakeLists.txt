cmake_minimum_required(VERSION 3.28)
project(GameEngine)

add_subdirectory(libs)

add_library(engine
        src/engine/engine.cpp
        src/engine/engine.hpp
        src/engine/gl.cpp
        src/engine/gl.hpp
        src/engine/window.cpp
        src/engine/window.hpp)
target_include_directories(engine PUBLIC src/)
target_link_libraries(engine PUBLIC spdlog::spdlog glad::glad glfw glm::glm)

add_library(engine::engine ALIAS engine)

add_subdirectory(example)
