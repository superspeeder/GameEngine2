#pragma once

#include <spdlog/spdlog.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdlib>
#include <cinttypes>
#include <string>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstring>

namespace engine {
    template<typename T>
    struct range {
        T offset;
        T size;
    };

    struct cpu_memory {
        size_t size;
        void* data;

        explicit inline cpu_memory(size_t size_) : size(size_) {
            data = std::malloc(size);
        };

        inline ~cpu_memory() {
            std::free(data);
        };

        cpu_memory(const cpu_memory&) = delete;
        cpu_memory& operator=(const cpu_memory&) = delete;

        cpu_memory(cpu_memory&&) = default;
        cpu_memory& operator=(cpu_memory&&) = default;
    };
}