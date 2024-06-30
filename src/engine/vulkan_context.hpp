#pragma once

#include "engine/engine.hpp"

namespace engine {

    class VulkanContext {
      public:
        struct Settings {
            std::string appName = "No Name";
            Version appVersion = { 1,0,0 };
        };

      private:
        explicit VulkanContext(const Settings& settings);
      public:
        inline static std::shared_ptr<VulkanContext> create(const Settings& settings) {
            return std::shared_ptr<VulkanContext>(new VulkanContext(settings));
        };

        ~VulkanContext();

        [[nodiscard]] bool canQueueFamilyPresent(uint32_t index);

        const vk::Instance       &getInstance() const;
        const vk::PhysicalDevice &getPhysicalDevice() const;
        const vk::Device         &getDevice() const;
        uint32_t                  getMainQueueFamily() const;
        uint32_t                  getTransferQueueFamily() const;
        const vk::Queue          &getMainQueue() const;
        const vk::Queue          &getTransferQueue() const;

      private:
        void createInstance();
        void selectPhysicalDevice();
        void findQueueFamilies();
        void createDevice();

        Settings m_Settings;

        vk::Instance m_Instance;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;

        uint32_t m_MainQueueFamily;
        uint32_t m_TransferQueueFamily;
        vk::Queue m_MainQueue;
        vk::Queue m_TransferQueue;
    };

} // namespace engine
