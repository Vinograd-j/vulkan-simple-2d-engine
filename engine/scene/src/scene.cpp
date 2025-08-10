#include "../include/scene.h"

#include "../../struct/storage-buffer.h"
#include "../objects/object-data.h"

Scene::Scene(const std::vector<std::shared_ptr<Renderable>>& objects) : _objects(objects)
{
    PrepareScene();
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

        StorageBufferObject obj {};
        obj._model = shape->GetModelMatrix();
        obj._color = shape->GetColor();
        _objectSSBO.push_back(obj);

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
}