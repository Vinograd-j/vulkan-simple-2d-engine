#version 450

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 fragColor;

struct StorageBufferObject {
    uint _type;
    mat4 _model;
    vec3 _color;
};

layout(set = 0, binding = 1) buffer ObjectBuffer {
    StorageBufferObject objects[];
};

layout(push_constant) uniform Push {
    uint objectIndex;
} pushData;

void main() {
    StorageBufferObject obj = objects[pushData.objectIndex];

    gl_Position = obj._model * vec4(inPosition, 0.0, 1.0);
    fragColor = obj._color;
}