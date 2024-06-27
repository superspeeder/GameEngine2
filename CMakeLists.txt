cmake_minimum_required(VERSION 3.28)
project(GameEngine)

add_subdirectory(libs)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.c src/*.cc src/*.cpp src/*.cxx src/*.c++)

add_library(engine ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC src/)
target_link_libraries(engine PUBLIC spdlog::spdlog glad::glad glfw glm::glm)

add_library(engine::engine ALIAS engine)
