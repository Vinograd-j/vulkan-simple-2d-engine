cmake_minimum_required(VERSION 3.31)
project(vulkan_simple_2d_engine)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif ()

find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui GIT_TAG master)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm GIT_TAG master)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator GIT_TAG master)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/SasLuca/MyGlad.git GIT_TAG master)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui glm vma glfw glad)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

add_subdirectory(file-manager)
add_subdirectory(backend)
add_subdirectory(engine)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(vulkan-engine WIN32 main.cpp)
else ()
    add_executable(vulkan-engine main.cpp)
endif ()

target_link_libraries(vulkan-engine PRIVATE backend engine glfw Vulkan::Vulkan)

#target_link_libraries(vulkan-engine Vulkan::Vulkan gui glm::glm GPUOpen::VulkanMemoryAllocator)
#target_include_directories(vulkan-engine PRIVATE 3rd-party)
#target_include_directories(vulkan-engine PUBLIC ${imgui_SOURCE_DIR})