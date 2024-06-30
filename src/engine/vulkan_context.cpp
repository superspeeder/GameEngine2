#include "vulkan_context.hpp"
#include <optional>
#include <vector>
#include <array>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace engine {
    constexpr vk::QueueFlags MAIN_QUEUE_FLAGS = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;


    VulkanContext::VulkanContext(const Settings& settings) : m_Settings{ settings }
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        createInstance();
        selectPhysicalDevice();
        findQueueFamilies();
        createDevice();
    }

    VulkanContext::~VulkanContext()
    {
        m_Device.destroy();
        m_Instance.destroy();
    }

    bool VulkanContext::canQueueFamilyPresent(uint32_t index)
    {
        return glfwGetPhysicalDevicePresentationSupport(m_Instance, m_PhysicalDevice, index);
    }

    void VulkanContext::createInstance()
    {
        auto appInfo = vk::ApplicationInfo(
            m_Settings.appName.c_str(),
            m_Settings.appVersion.makeVulkanVersion(),
            NAME.data(),
            VERSION.makeVulkanVersion(),
            VK_API_VERSION_1_3);

        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        auto createInfo = vk::InstanceCreateInfo(
            {},
            &appInfo,
            0, nullptr,
            count, extensions);

        m_Instance = vk::createInstance(createInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
    }

    void VulkanContext::selectPhysicalDevice()
    {
        auto physicalDevices = m_Instance.enumeratePhysicalDevices();
        m_PhysicalDevice = physicalDevices[0];
    }

    void VulkanContext::findQueueFamilies()
    {
        auto queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

        std::optional<uint32_t> mainQueueFamily;
        uint32_t index = 0;

        std::optional<uint32_t> exclusiveTransferSparse;
        std::optional<uint32_t> exclusiveTransfer;

        for (const auto& properties : queueFamilyProperties) {
            if (flags_check(properties.queueFlags, MAIN_QUEUE_FLAGS) && canQueueFamilyPresent(index) && !mainQueueFamily.has_value()) {
                mainQueueFamily = index;
            }
            else if (!exclusiveTransferSparse.has_value() &&
                     flags_check(properties.queueFlags, vk::QueueFlagBits::eTransfer) &&
                     !(properties.queueFlags & (MAIN_QUEUE_FLAGS | vk::QueueFlagBits::eVideoDecodeKHR | vk::QueueFlagBits::eVideoEncodeKHR))) {
                if (flags_check(properties.queueFlags, vk::QueueFlagBits::eSparseBinding)) {
                    exclusiveTransferSparse = index;
                }
                else {
                    exclusiveTransfer = index;
                }
            }

            if (mainQueueFamily.has_value() && exclusiveTransferSparse.has_value()) break;
        }

        if (!mainQueueFamily.has_value()) throw std::runtime_error("Failed to find main queue family");

        m_MainQueueFamily = mainQueueFamily.value();
        m_TransferQueueFamily = exclusiveTransferSparse.value_or(exclusiveTransfer.value_or(mainQueueFamily.value()));
    }

    void VulkanContext::createDevice()
    {
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
        std::array<float, 1> queuePriorities = { 1.0f };

        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), m_MainQueueFamily, queuePriorities);
        if (m_TransferQueueFamily != m_MainQueueFamily) {
            queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), m_TransferQueueFamily, queuePriorities);
        }

        std::vector<const char*> extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        vk::PhysicalDeviceFeatures2 f2{};
        f2.features.fillModeNonSolid = true;
        f2.features.geometryShader = true;
        f2.features.tessellationShader = true;
        f2.features.largePoints = true;
        f2.features.wideLines = true;
        f2.features.drawIndirectFirstInstance = true;
        f2.features.multiDrawIndirect = true;

        vk::PhysicalDeviceVulkan11Features v11f{};
        v11f.variablePointers = true;

        vk::PhysicalDeviceVulkan12Features v12f{};
        v12f.timelineSemaphore = true;

        vk::PhysicalDeviceVulkan13Features v13f{};
        v13f.dynamicRendering = true;
        v13f.inlineUniformBlock = true;
        v13f.synchronization2 = true;

        f2.pNext = &v11f;
        v11f.pNext = &v12f;
        v12f.pNext = &v13f;

        auto createInfo = vk::DeviceCreateInfo({}, queueCreateInfos, {}, extensions, nullptr, &f2);
        m_Device = m_PhysicalDevice.createDevice(createInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);

        m_MainQueue = m_Device.getQueue(m_MainQueueFamily, 0);
        m_TransferQueue = m_Device.getQueue(m_TransferQueueFamily, 0);
    }

    const vk::Instance &VulkanContext::getInstance() const {
        return m_Instance;
    }

    const vk::PhysicalDevice &VulkanContext::getPhysicalDevice() const {
        return m_PhysicalDevice;
    }

    const vk::Device &VulkanContext::getDevice() const {
        return m_Device;
    }

    uint32_t VulkanContext::getMainQueueFamily() const {
        return m_MainQueueFamily;
    }

    uint32_t VulkanContext::getTransferQueueFamily() const {
        return m_TransferQueueFamily;
    }

    const vk::Queue &VulkanContext::getMainQueue() const {
        return m_MainQueue;
    }

    const vk::Queue &VulkanContext::getTransferQueue() const {
        return m_TransferQueue;
    }
} // namespace engine
