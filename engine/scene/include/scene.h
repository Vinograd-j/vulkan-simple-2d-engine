#pragma once

#include <memory>
#include <vector>

#include "../../struct/object-data.h"
#include "../objects/include/renderable.h"

class Scene {

private:

    std::vector<std::shared_ptr<Renderable>> _objects;

    std::vector<ObjectData> _objectSSBO;

    std::vector<Vertex> _allVertices;
    std::vector<uint16_t> _allIndices;

public:

    explicit Scene(const std::vector<std::shared_ptr<Renderable>>& objects);

    std::vector<ObjectData> GetObjectsSSBO() const { return _objectSSBO; }

    std::vector<Vertex> GetAllVertices() const { return _allVertices; }

    std::vector<uint16_t> GetAllIndices() const { return _allIndices; }

    ~Scene() = default;

private:

    void PrepareScene();

};
