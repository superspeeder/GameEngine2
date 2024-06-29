#pragma once

#include <glad/gl.h>

#include <vector>
#include <array>
#include <string>

namespace glw {
    void load();

    std::vector<std::string> GetExtensions();
    std::vector<std::string> GetSpirVExtensions();

    int GetInteger(GLenum pname);

    template<size_t N>
    std::array<int, N> GetIntegerN(GLenum pname) {
        static_assert(N > 1, "For single integers, use glw::GetInteger(GLenum)");
        std::array<int, N> arr;
        glGetIntegerv(pname, arr.data());
        return arr;
    };

    int GetIntegeri(GLenum pname, GLuint i);

    std::string GetString(GLenum pname);
    std::string GetStringi(GLenum pname, GLuint i);

    class Buffer {
      public:
        Buffer(size_t size, const void* data = nullptr);
        ~Buffer();

        void set()

      private:
        size_t m_CurrentSize;

    };
}