#include "../include/scene.h"

#include "../../struct/object-data.h"

Scene::Scene(const std::vector<std::unique_ptr<Renderable>> &objects) : _objects(objects)
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

        ObjectData obj {};
        obj._model = shape->GetModelMatrix();
        obj._color = shape->GetColor();
        obj._type = static_cast<int>(shape->GetShapeType());
        obj._vertexOffset = vertexOffset;
        obj._indexOffset = indexOffset;
        obj._indexCount = indices.size();

        _objectSSBO.push_back(obj);

        vertexOffset += vertices.size();
        indexOffset += indices.size();
    }

    _allVertices = allVertices;
    _allIndices = allIndices;
}

Scene::~Scene()
{
    for (auto& object : _objects)
        object.reset();
}