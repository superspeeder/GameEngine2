#include <engine/engine.hpp>
#include <engine/gl.hpp>

#include <iostream>

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, false);
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, false);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glw::load();

    auto exts = glw::GetExtensions();
    std::cout << "Extensions (" << exts.size() << ")\n";
    for (const auto& s : exts) {
        std::cout << "- " << s << '\n';
    }
    std::cout << '\n';

    auto spv_exts = glw::GetSpirVExtensions();
    std::cout << "SPIR-V Extensions (" << spv_exts.size() << ")\n";
    for (const auto& s : spv_exts) {
        std::cout << "- " << s << '\n';
    }

    auto gl_ver = glw::GetString(GL_VERSION);
    auto gl_vendor = glw::GetString(GL_VENDOR);
    auto gl_renderer = glw::GetString(GL_RENDERER);
    auto glsl_version = glw::GetString(GL_SHADING_LANGUAGE_VERSION);

    std::cout << "=======================================================================================================================\n";
    std::cout << "GL Info:\n";
    std::cout << "                   Version: " << gl_ver << '\n';
    std::cout << "                    Vendor: " << gl_vendor << '\n';
    std::cout << "                  Renderer: " << gl_renderer << '\n';
    std::cout << "  Shading Language Version: " << glsl_version << '\n';
    std::cout << "=======================================================================================================================\n";


    std::vector<float> verts = {0,0,0,1,0,0,0,1,0};
    auto vertex_buffer = glw::Buffer::create(verts, glw::Buffer::Usage::StaticDraw);
    auto vertex_array = glw::VertexArray::create();

    vertex_buffer->bind(glw::Buffer::Target::Array);
    vertex_array->bindVertexBuffer(vertex_buffer, {3});


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vertex_array->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}