add_library(backend STATIC
        vulkan/src/Instance.cpp
        vulkan/include/Instance.h
        vulkan/src/vulkan-application.cpp
        vulkan/include/vulkan-application.h
        vulkan/include/window.h
        vulkan/src/window.cpp
        vulkan/src/surface.cpp
        vulkan/include/surface.h
        vulkan/include/physical-device.h
        vulkan/src/physical-device.cpp
        vulkan/include/logical-device.h
        vulkan/src/logical-device.cpp
        vulkan/include/allocator.h
        vulkan/src/allocator.cpp
        vulkan/include/queue-family-indices.h
        vulkan/include/swap-chain-support-details.h
        ../engine/gui/src/im-gui-command-buffer-recorder.cpp
        ../engine/gui/include/im-gui-command-buffer-recorder.h
        vulkan/buffers/src/storage-buffer.cpp
        vulkan/buffers/include/storage-buffer.h
)

target_include_directories(backend PUBLIC vulkan/include)

target_link_libraries(backend PUBLIC Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator glad glfw imgui)