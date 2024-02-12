#pragma once

#include "Types.h"
#include "Device.h"
#include "Buffer.h"

namespace Framework
{
	namespace Vulkan
	{
		class CCommandBufferPool;

		class CImage
		{
		public:
			        CImage() = default;
			        CImage(CDevice&, const VkPhysicalDeviceMemoryProperties&, VkImageUsageFlags, VkFormat, uint32, uint32, VkMemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			        CImage(const CImage&) = delete;
			        CImage(CImage&&);
			virtual ~CImage();
			
			bool IsEmpty() const;
			void Reset();
			
			uint32 GetLinearSize() const;

			VkImageView CreateImageView();

			CBuffer CreateFillStagingBuffer(const VkPhysicalDeviceMemoryProperties&, const void*) const;
			void RecordFill(VkCommandBuffer, const CBuffer&);

			void SetLayout(VkQueue, CCommandBufferPool&, VkImageLayout, VkAccessFlags);
			void Fill(VkQueue, CCommandBufferPool&, const VkPhysicalDeviceMemoryProperties&, const void*);

			CImage& operator =(const CImage&) = delete;
			CImage& operator =(CImage&&);
			         operator VkImage() const;

		private:
			void Create(const VkPhysicalDeviceMemoryProperties&, VkImageUsageFlags, VkFormat, uint32, uint32);
			void MoveFrom(CImage&&);
			
			CDevice* m_device = nullptr;
			VkImage m_handle = VK_NULL_HANDLE;
			VkDeviceMemory m_memory = VK_NULL_HANDLE;
			VkMemoryPropertyFlags m_properties = 0;

			VkFormat m_format = VK_FORMAT_UNDEFINED;
			uint32 m_width = 0;
			uint32 m_height = 0;
		};
	}
}
