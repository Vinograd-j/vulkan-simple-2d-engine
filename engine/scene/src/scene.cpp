#include "../include/scene.h"

#include <glm/ext/matrix_transform.hpp>

#include "../objects/object-data.h"

Scene::Scene(const std::vector<std::shared_ptr<Renderable>>& objects, const CommandPool* commandPool, const Allocator* allocator, const LogicalDevice* device)
                                                                                                                                        : _objects(objects),
                                                                                                                                          _commandPool(commandPool),
                                                                                                                                          _allocator(allocator),
                                                                                                                                          _device(device)
{
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateSSBO();
    CreateViewProjectionBuffers();
}

Scene::Scene(const CommandPool* commandPool, const Allocator* allocator, const LogicalDevice* device)  : _commandPool(commandPool),
                                                                                                         _allocator(allocator),
                                                                                                         _device(device)
{
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateSSBO();
    CreateViewProjectionBuffers();
}

void Scene::AddObject(const std::shared_ptr<Renderable>& object)
{
    auto indices = object->GetIndices();
    for (auto& idx : indices)
        idx += _currentVertexOffset;

    _objects.push_back(object);
    _vertexBuffer->AddVertices(object->GetVertices(), sizeof(Vertex) * _currentVertexOffset);
    _indexBuffer->AddIndices(indices, sizeof(uint16_t) * _currentIndexOffset);

    ObjectData obj{};
    obj._type = static_cast<int>(object->GetShapeType());
    obj._indexOffset = _currentIndexOffset;
    obj._indexCount  = static_cast<uint32_t>(object->GetIndices().size());
    _objectDatas.push_back(obj);

    _currentVertexOffset += static_cast<uint32_t>(object->GetVertices().size());
    _currentIndexOffset += static_cast<uint32_t>(object->GetIndices().size());

    StorageBufferObject storageBuffer {};
    storageBuffer._objectId = static_cast<uint32_t>(object->GetShapeType());
    storageBuffer._model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f));
    storageBuffer._color = glm::vec3(1, 1, 1);
    _bufferObjects.push_back(storageBuffer);
}

void Scene::CreateSSBO()
{
    _storageBuffer.resize(2);
    for (int i = 0; i < _storageBuffer.size(); ++i)
        _storageBuffer[i] = std::make_unique<StorageBuffer>(_allocator, _commandPool, _device);
}

void Scene::CreateStorageBufferObjects()
{
    std::vector<StorageBufferObject> bufferObjects;

    for (const auto& object : _objects)
    {
        StorageBufferObject storageBuffer {};
        storageBuffer._model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f));
        storageBuffer._color = glm::vec3(1, 1, 1);
        bufferObjects.push_back(storageBuffer);
    }

    _bufferObjects = bufferObjects;
}

void Scene::CreateVertexBuffer()
{
    if (_vertexBuffer)
        _vertexBuffer.reset();

    _vertexBuffer = std::make_unique<VertexBuffer>(_allocator, _commandPool, _device);
}

void Scene::CreateIndexBuffer()
{
    if (_indexBuffer)
        _indexBuffer.reset();

    _indexBuffer = std::make_unique<IndexBuffer>(_allocator, _commandPool, _device);
}

void Scene::CreateViewProjectionBuffers()
{
    _viewProjBuffers.resize(2);

    for (auto& buffer : _viewProjBuffers)
        buffer = std::make_shared<UniformBuffer>(_allocator, _commandPool, _device, ViewProjectionBuffer {});
}

Scene::~Scene()
{
    _vertexBuffer.reset();
    _indexBuffer.reset();

    for (auto& buffer : _storageBuffer)
        buffer.reset();
    for (auto& buffer : _viewProjBuffers)
        buffer.reset();

    for (auto& object : _objects)
        object.reset();
}