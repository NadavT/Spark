#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark
{
	class VulkanFramebuffer2D : public VulkanFramebuffer
	{
	public:
		VulkanFramebuffer2D(VulkanContext& context, bool firstLayer = false);
		virtual ~VulkanFramebuffer2D();

		virtual void cleanup();
		virtual void recreate();

	private:
		void createRenderPass(bool firstLayer);
		void createColorResources();
		void createSwapchainFramebuffers();

	private:
		bool m_firstLayer;
		VkImage m_colorImage;
		VkDeviceMemory m_colorImageMemory;
		VkImageView m_colorImageView;
	};
}
