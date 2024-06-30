#include "window.hpp"

namespace engine {
    Window::Window(const Settings &settings, const std::shared_ptr<VulkanContext> &vulkan_context) : m_VulkanContext(vulkan_context) {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(settings.size.x, settings.size.y, settings.title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_Window, this);

        VkSurfaceKHR s;
        glfwCreateWindowSurface(m_VulkanContext->getInstance(), m_Window, nullptr, &s);
        m_Surface = s;
    }

    Window::~Window() {
        glfwDestroyWindow(m_Window);
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(m_Window);
    }

    GLFWwindow *Window::getWindow() const {
        return m_Window;
    }

    const vk::SurfaceKHR &Window::getSurface() const {
        return m_Surface;
    }

    void Window::recreateSwapchain() {
        vk::SwapchainKHR old = nullptr;

        if (m_Swapchain) {
            m_VulkanContext->getDevice().waitIdle();
            for (const auto &iv : m_ImageViews)
                m_VulkanContext->getDevice().destroy(iv);

            old = m_Swapchain;
        }

        auto caps         = m_VulkanContext->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface);
        auto formats      = m_VulkanContext->getPhysicalDevice().getSurfaceFormatsKHR(m_Surface);
        auto presentModes = m_VulkanContext->getPhysicalDevice().getSurfacePresentModesKHR(m_Surface);

        vk::PresentModeKHR chosenPM = vk::PresentModeKHR::eFifo;
        for (const auto &pm : presentModes) {
            if (pm == vk::PresentModeKHR::eMailbox) {
                chosenPM = vk::PresentModeKHR::eMailbox;
                break;
            }
        }

        vk::SurfaceFormatKHR chosenIF = formats[0];
        for (const auto &fmt : formats) {
            if (fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && fmt.format == vk::Format::eB8G8R8A8Srgb) {
                chosenIF = fmt;
                break;
            }
            if (fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && fmt.format == vk::Format::eR8G8B8A8Srgb) {
                chosenIF = fmt;
            }
        }

        vk::Extent2D extent = caps.currentExtent;

        if (extent.height == UINT32_MAX) {
            int w, h;
            glfwGetFramebufferSize(m_Window, &w, &h);
            extent.width  = std::clamp<uint32_t>(w, caps.minImageExtent.width, caps.maxImageExtent.width);
            extent.height = std::clamp<uint32_t>(h, caps.minImageExtent.height, caps.maxImageExtent.height);
        }

        vk::SwapchainCreateInfoKHR swci{};
        swci.oldSwapchain     = old;
        swci.clipped          = true;
        swci.compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swci.preTransform     = caps.currentTransform;
        swci.imageSharingMode = vk::SharingMode::eExclusive;
        swci.minImageCount    = caps.minImageCount + 1;
        if (caps.maxImageCount > 0 && swci.minImageCount > caps.maxImageCount) {
            swci.minImageCount = caps.maxImageCount;
        }
        swci.presentMode      = chosenPM;
        swci.imageFormat      = chosenIF.format;
        swci.imageColorSpace  = chosenIF.colorSpace;
        swci.imageArrayLayers = 1;
        swci.imageExtent      = extent;
        swci.imageUsage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment;
        swci.surface          = m_Surface;

        m_Swapchain       = m_VulkanContext->getDevice().createSwapchainKHR(swci);
        m_SwapchainImages = m_VulkanContext->getDevice().getSwapchainImagesKHR(m_Swapchain);

        m_Format      = swci.imageFormat;
        m_ColorSpace  = swci.imageColorSpace;
        m_Extent      = swci.imageExtent;
        m_PresentMode = swci.presentMode;

        m_ImageViews.clear();
        m_ImageViews.resize(m_SwapchainImages.size());
        for (uint32_t i = 0; i < m_SwapchainImages.size(); i++) {
            m_ImageViews[i] = m_VulkanContext->getDevice().createImageView(
                vk::ImageViewCreateInfo({}, m_SwapchainImages[i], vk::ImageViewType::e2D, m_Format,
                                        {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA},
                                        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}));
        }
    }
} // namespace engine
