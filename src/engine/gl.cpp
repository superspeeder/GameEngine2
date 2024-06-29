#include "engine/gl.hpp"

#include <GLFW/glfw3.h>

namespace glw {
    void load() {
        gladLoadGL(glfwGetProcAddress);
    }

    std::vector<std::string> GetExtensions() {
        std::vector<std::string> extensions;
        int count = GetInteger(GL_NUM_EXTENSIONS);
        extensions.reserve(count);
        for (unsigned int i = 0 ; i < count ; i++) {
            extensions.push_back(GetStringi(GL_EXTENSIONS, i));
        }

        return extensions;
    }

    std::vector<std::string> GetSpirVExtensions() {
        std::vector<std::string> extensions;
        int count = GetInteger(GL_NUM_SPIR_V_EXTENSIONS);
        extensions.reserve(count);
        for (unsigned int i = 0 ; i < count ; i++) {
            extensions.push_back(GetStringi(GL_SPIR_V_EXTENSIONS, i));
        }

        return extensions;
    }

    int GetInteger(GLenum pname) {
        int n;
        glGetIntegerv(pname, &n);
        return n;
    }

    int GetIntegeri(GLenum pname, GLuint i) {
        int n;
        glGetIntegeri_v(pname, i, &n);
        return n;
    }

    std::string GetString(GLenum pname) {
        return reinterpret_cast<const char*>(glGetString(pname));
    }

    std::string GetStringi(GLenum pname, GLuint i) {
        return reinterpret_cast<const char*>(glGetStringi(pname, i));
    }
}
