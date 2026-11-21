#version 460 core

in vec2 vTexCoord;

uniform sampler2D scene;

out vec4 oFragColor;

void main() {
    oFragColor = texture(scene, vTexCoord);
    // oFragColor = vec4(vTexCoord, 0, 1);
}
