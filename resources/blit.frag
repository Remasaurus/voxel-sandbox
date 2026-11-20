#version 460 core

in vec2 vTexCoord;

uniform sampler2D screen;

out vec4 oFragColor;

void main() { oFragColor = vec4(vTexCoord, 0, 1); }
