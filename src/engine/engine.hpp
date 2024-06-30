#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <vulkan/vulkan.hpp>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdlib>
#include <cinttypes>
#include <string>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <string_view>

namespace engine {
    template<typename T>
    struct range {
        T offset;
        T size;
    };

    struct CpuMemory {
        size_t size;
        void* data;

        explicit inline CpuMemory(size_t size_) : size(size_) {
            data = std::malloc(size);
        };

        inline ~CpuMemory() {
            std::free(data);
        };

        CpuMemory(const CpuMemory&) = delete;
        CpuMemory& operator=(const CpuMemory&) = delete;

        CpuMemory(CpuMemory&&) = default;
        CpuMemory& operator=(CpuMemory&&) = default;
    };

    template<typename T>
    constexpr bool flags_check(const vk::Flags<T>& flags, const vk::Flags<T>& check) noexcept {
        return (flags & check) == check;
    };

    template<typename T>
    constexpr bool flags_check(const vk::Flags<T>& flags, const T& check) noexcept {
        return (flags & check) == check;
    };

    struct Version {
        uint32_t major, minor, patch;

        [[nodiscard]] constexpr uint32_t makeVulkanVersion() const noexcept {
            return VK_MAKE_API_VERSION(0, major, minor, patch);
        };
    };

    constexpr Version VERSION = { 0, 1, 0 };
    constexpr std::string_view NAME = "Unnamed GameEngine";
}