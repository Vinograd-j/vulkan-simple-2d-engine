#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(std430, set = 0, binding = 1) buffer ObjectBuffer {
    ObjectData objects[];
};

void main() {
    ObjectData obj = objects[gl_InstanceIndex];

    gl_Position = obj._model * vec4(inPosition, 0.0, 1.0);
    fragColor = obj._color.rgb;
}