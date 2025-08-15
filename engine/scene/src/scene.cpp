#include "../include/scene.h"

#include <glm/ext/matrix_transform.hpp>

#include "../objects/object-data.h"

Scene::Scene(const std::vector<std::shared_ptr<Renderable>>& objects, const CommandPool* commandPool, const Allocator* allocator, const LogicalDevice* device)
                                                                                                                                        : _objects(objects),
                                                                                                                                          _commandPool(commandPool),
                                                                                                                                          _allocator(allocator),
                                                                                                                                          _device(device)
{
    PrepareScene();
}

void Scene::AddObject(const std::shared_ptr<Renderable> &object)
{
    _objects.push_back(object);
    _needsRebuild = true;
}

void Scene::RequestRebuild()
{
    if (_needsRebuild)
    {
        vkDeviceWaitIdle(_device->GetDevice());
        PrepareScene();
        _needsRebuild = false;
    }
}

void Scene::PrepareScene()
{
    std::vector<Vertex> allVertices;
    std::vector<uint16_t> allIndices;

    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;

    for (const auto& shape : _objects)
    {
        const auto& vertices = shape->GetVertices();
        const auto& indices = shape->GetIndices();

        allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());

        for (auto index : indices)
            allIndices.push_back(index + vertexOffset);

        ObjectData objectData {};
        objectData._type = static_cast<int>(shape->GetShapeType());
        objectData._indexOffset = indexOffset;
        objectData._indexCount = indices.size();
        _objectDatas.push_back(objectData);

        vertexOffset += vertices.size();
        indexOffset += indices.size();
    }

    _allVertices = allVertices;
    _allIndices = allIndices;

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateStorageBufferObjects();
    CreateSSBO();
}

void Scene::CreateSSBO()
{
    _storageBuffer.resize(2);
    for (int i = 0; i < _storageBuffer.size(); ++i)
        _storageBuffer[i] = std::make_unique<StorageBuffer>(_allocator, _commandPool, _device, _bufferObjects);
}

void Scene::CreateStorageBufferObjects()
{
    std::vector<StorageBufferObject> bufferObjects;

    for (const auto& object : _objects)
    {
        StorageBufferObject storageBuffer {};
        storageBuffer._model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        storageBuffer._color = glm::vec3(1, 1, 1);
        bufferObjects.push_back(storageBuffer);
    }

    _bufferObjects = bufferObjects;
}

void Scene::CreateVertexBuffer()
{
    if (_vertexBuffer)
        _vertexBuffer.reset();

    _vertexBuffer = std::make_unique<VertexBuffer>(_allocator, _allVertices, _commandPool, _device);
}

void Scene::CreateIndexBuffer()
{
    if (_indexBuffer)
        _indexBuffer.reset();

    _indexBuffer = std::make_unique<IndexBuffer>(_allocator, _allIndices, _commandPool, _device);
}

Scene::~Scene()
{
    _vertexBuffer.reset();
    _indexBuffer.reset();

    for (auto& buffer : _storageBuffer)
        buffer.reset();

    for (auto& object : _objects)
        object.reset();
}