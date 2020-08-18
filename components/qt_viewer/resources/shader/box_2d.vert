#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
uniform vec2 box_min;
uniform vec2 box_max;
out vec4 vColor;
void main() {
    gl_Position = vec4(position.xy * (box_max - box_min) + box_min, 0, 1);
    vColor = vec4(color, 1.0);
}
