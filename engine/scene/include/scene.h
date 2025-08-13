#pragma once

#include <memory>
#include <vector>

#include "../objects/object-data.h"
#include "../objects/include/renderable.h"

class Scene {

private:

    std::vector<std::shared_ptr<Renderable>> _objects;

    std::vector<ObjectData> _objectDatas;

    std::vector<Vertex> _allVertices;
    std::vector<uint16_t> _allIndices;

public:

    explicit Scene(const std::vector<std::shared_ptr<Renderable>>& objects);
    explicit Scene() = default;

    void AddObject(const std::shared_ptr<Renderable>& object);

    std::vector<ObjectData> GetObjectData() const { return _objectDatas; }

    std::vector<Vertex> GetAllVertices() const { return _allVertices; }
    std::vector<uint16_t> GetAllIndices() const { return _allIndices; }

    std::vector<std::shared_ptr<Renderable>> GetObjects() const { return _objects; }

    ~Scene() = default;

private:

    void PrepareScene();

};
