#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

struct ObjectData {
    mat4 _model;
    vec3 _color;

    int _type;
    int _indexOffset;
    int _indexCount;
};

layout(set = 0, binding = 1) buffer ObjectBuffer {
    ObjectData objects[];
};

layout(push_constant) uniform Push {
    uint objectIndex;
} pushData;

void main() {
    ObjectData obj = objects[pushData.objectIndex];

    gl_Position = obj._model * vec4(inPosition, 0.0, 1.0);
    fragColor = obj._color;
}