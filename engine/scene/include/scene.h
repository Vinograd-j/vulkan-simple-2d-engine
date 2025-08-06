#pragma once

#include <memory>
#include <vector>

#include "../../struct/object-data.h"
#include "../objects/include/renderable.h"

class Scene {

private:

    std::vector<std::unique_ptr<Renderable>> _objects;

    std::vector<ObjectData> _objectSSBO;

    std::vector<Vertex> _allVertices;
    std::vector<uint32_t> _allIndices;

public:

    explicit Scene(const std::vector<std::unique_ptr<Renderable>>& objects);

    std::vector<ObjectData> GetObjectsSSBO() const { return _objectSSBO; }

    std::vector<Vertex> all_vertices() const { return _allVertices; }

    std::vector<uint32_t> all_indices() const { return _allIndices; }

    ~Scene();

private:

    void PrepareScene();

};
