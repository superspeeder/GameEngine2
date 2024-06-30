#pragma once

#include "engine/engine.hpp"
#include "engine/vulkan_context.hpp"

#include <functional>
#include <string>

namespace engine {

    class Window {
      public:
        struct Settings {
            std::string title = "Window";
            glm::ivec2  size  = {800, 600};
        };

      private:
        Window(const Settings &settings, const std::shared_ptr<VulkanContext> &vulkan_context);

      public:
        static inline std::shared_ptr<Window> create(const Settings &settings, const std::shared_ptr<VulkanContext> &vulkan_context) {
            return std::shared_ptr<Window>(new Window(settings, vulkan_context));
        };

        ~Window();

        [[nodiscard]] bool shouldClose();

        [[nodiscard]] GLFWwindow           *getWindow() const;
        [[nodiscard]] const vk::SurfaceKHR &getSurface() const;

        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

      private:
        GLFWwindow                    *m_Window;
        std::shared_ptr<VulkanContext> m_VulkanContext;
        vk::SurfaceKHR                 m_Surface;

        void recreateSwapchain();

        vk::SwapchainKHR           m_Swapchain;
        std::vector<vk::Image>     m_SwapchainImages;
        vk::ColorSpaceKHR          m_ColorSpace;
        vk::Extent2D               m_Extent;
        vk::Format                 m_Format;
        vk::PresentModeKHR         m_PresentMode;
        std::vector<vk::ImageView> m_ImageViews;

        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Fence>     m_InFlightFences;
    };

    inline void pollEvents() {
        glfwPollEvents();
    };

} // namespace engine
