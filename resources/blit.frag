#version 460 core

layout (location = 0) in vec2 vTexCoord;

uniform sampler2D screen;

out vec4 oFragColor;

void main() {
    oFragColor = vec4(vTexCoord, 0, 1);
}
