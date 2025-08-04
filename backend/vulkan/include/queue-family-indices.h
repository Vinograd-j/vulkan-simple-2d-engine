#pragma once

#include <optional>
#include <vector>
#include <cstdint>

enum FamilyType
{
    GRAPHICS,
    COMPUTE,
    TRANSFER,
    SPARCE_BINDING,
    PRESENT
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> _graphicsFamily;

    std::optional<uint32_t> _computeFamily;

    std::optional<uint32_t> _transferFamily;

    std::optional<uint32_t> _sparceBindingFamily;

    std::optional<uint32_t> _presentFamily;


    bool IsComplete(bool requireCompute = true, bool requireTransfer = true) const
    {
        return _graphicsFamily.has_value() &&
               (!requireCompute || _computeFamily.has_value()) &&
               (!requireTransfer || _transferFamily.has_value()) &&
               _presentFamily.has_value();
    }

    bool IsUnifiedGraphicsAndPresentQueues() const
    {
        return _graphicsFamily.has_value() && _presentFamily.has_value() && _graphicsFamily.value() == _presentFamily.value();
    }

    std::vector<std::optional<uint32_t>> GetAllFamilies() const
    {
        std::vector<std::optional<uint32_t>> families;

        families.push_back(_graphicsFamily);
        families.push_back(_computeFamily);
        families.push_back(_transferFamily);
        families.push_back(_sparceBindingFamily);
        families.push_back(_presentFamily);

        return families;
    }

};