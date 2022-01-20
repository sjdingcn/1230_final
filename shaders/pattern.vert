#version 330 core

layout(location = 0) in vec3 position;
layout(location = 5) in vec2 inUV;

out vec2 uv;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

void main() {
    uv = inUV;
    vec4 position_cameraSpace = v * m * vec4(position.x, position.y+0.01, position.z, 1.0);
    gl_Position = p * position_cameraSpace;
}
