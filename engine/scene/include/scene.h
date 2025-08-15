#pragma once

#include <memory>
#include <vector>

#include "../../../backend/vulkan/buffers/include/index-buffer.h"
#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../../../backend/vulkan/buffers/include/vertex-buffer.h"
#include "../../struct/storage-buffer.h"
#include "../objects/object-data.h"
#include "../objects/include/renderable.h"

class Scene
{

private:

    std::vector<std::shared_ptr<Renderable>> _objects;

    std::vector<ObjectData> _objectDatas;

    std::vector<Vertex> _allVertices;
    std::vector<uint16_t> _allIndices;

    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::unique_ptr<VertexBuffer> _vertexBuffer;

    std::vector<StorageBufferObject> _bufferObjects;
    std::vector<std::shared_ptr<StorageBuffer>> _storageBuffer;

private:

    const CommandPool* const _commandPool;
    const Allocator* const _allocator;
    const LogicalDevice* const _device;

    bool _needsRebuild;

public:

    explicit Scene(const std::vector<std::shared_ptr<Renderable>>& objects, const CommandPool* commandPool, const Allocator* allocator, const LogicalDevice* device);

    explicit Scene(const CommandPool* commandPool, const Allocator* allocator, const LogicalDevice* device) : _commandPool(commandPool),
                                                                                                              _allocator(allocator),
                                                                                                              _device(device),
                                                                                                              _needsRebuild(false){}

    void AddObject(const std::shared_ptr<Renderable>& object);
    void RequestRebuild();

    std::vector<ObjectData> GetObjectData() const { return _objectDatas; }

    const VertexBuffer* GetVertexBuffer() const { return _vertexBuffer.get(); }
    const IndexBuffer* GetIndexBuffer() const { return _indexBuffer.get(); }

    std::vector<StorageBufferObject>& GetBufferObjects() { return _bufferObjects; }
    size_t GetBufferObjectsSize() const { return _bufferObjects.size(); }
    std::vector<std::shared_ptr<StorageBuffer>> GetStorageBuffers() const {return _storageBuffer; }

    ~Scene();

private:

    void PrepareScene();

    void CreateSSBO();

    void CreateStorageBufferObjects();

    void CreateVertexBuffer();

    void CreateIndexBuffer();

};