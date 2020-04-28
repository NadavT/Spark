#pragma once

#include "spark/core/window.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace Spark
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext
	{
	public:
		VulkanContext(const Window& window);
		~VulkanContext();
		VulkanContext(const VulkanContext& other) = delete;
		VulkanContext(VulkanContext&& other) = delete;
		VulkanContext& operator=(const VulkanContext& other) = delete;
		VulkanContext& operator=(VulkanContext&& other) = delete;

	private:
		void createInstance();
		void setupDebugMessenger();
		void createSurface(const Window& window);
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSwapChain(const Window& window);
		void createImageViews();
		void createRenderPass();

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		static std::vector<const char*> getRequiredExtensions();
		static bool checkValidationLayerSupport();
		VkSampleCountFlagBits getMaxUsableSampleCount();

	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkSurfaceKHR m_surface;

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		VkDevice m_device;
		int maxDescriptorSetArrayLength;

		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		VkSwapchainKHR m_swapChain;
		std::vector<VkImage> m_swapChainImages;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;
		std::vector<VkImageView> m_swapChainImageViews;

		VkRenderPass m_renderPass;

		VkCommandPool m_commandPool;
	};
}
