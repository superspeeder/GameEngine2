file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS src/*.c src/*.cc src/*.cpp src/*.cxx src/*.c++)

add_executable(example ${EXAMPLE_SOURCES})
target_include_directories(example PRIVATE src/)
target_link_libraries(example PRIVATE engine::engine)