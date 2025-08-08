#pragma once

#include <memory>

#include "allocator.h"
#include "gui.h"
#include "scene-command-buffer-recorder.h"
#include "scene.h"
#include "synchronization.h"
#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../../gui/include/im-gui.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"
#include "../../renderer/include/renderer.h"

constexpr int FRAMES_IN_FLIGHT = 2;

class SceneDrawer : Renderer
{

private:

    const Allocator* _allocator;

    const CommandPool* const _commandPool;

    CommandBuffers _commandBuffers;

    const Gui* const _gui;

    std::unique_ptr<Scene> _scene;

private:

    Synchronization _syncObjects;

    std::vector<VkImageView> _imageViews;

    std::vector<VkImageLayout> _swapchainImageLayouts;

    std::unique_ptr<SceneCommandBufferRecorder> _recorder;

    VkDescriptorSetLayout _descriptorSetLayout;
    VkDescriptorPool _descriptorPool;
    std::vector<VkDescriptorSet> _descriptorSets;

    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::vector<std::unique_ptr<StorageBuffer>> _storageBuffer;

private:

    uint32_t _currentFrame = 0;

public:

    explicit SceneDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device, const VkDescriptorSetLayout& layout, const Gui* gui);

    void DrawFrame() override;

    ~SceneDrawer() override;

    std::vector<VkImageView> GetImageViews() const { return _imageViews; }

private:

    VkImageSubresourceRange GetImageSubresourceRange() const;

    VkComponentMapping GetComponentMapping() const;

    void RecreateSwapchain();

    void CreateBufferRecorder();

    void CreateSSBO();
    void CreateDescriptorSets();
    void CreateDescriptorPool();

    void CreateScene();

    void CreateVertexBuffer();
    void CreateIndexBuffer();
};