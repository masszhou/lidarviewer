#version 330
// reference:  taken from PPTK project
// originally is left hand coordinates
// here I modified to OpenGL canvas coordinates, i.e. right hand coord

uniform vec3 eye;
uniform vec3 right;
uniform vec3 up;
uniform vec3 view;
uniform float z_floor;
uniform float r;
uniform float t;

layout(location = 0) in vec3 position;

out vec2 floor_coord;

void main() {
  vec2 image_coord = (position.xy - 0.5) * 2.0; // setup canvas
  vec3 o = eye + image_coord.x * r * right + image_coord.y * t * up;  // move reference point from center to top right w.r.t. vfov
  float d = (o.y - z_floor) / view.y;
  floor_coord = -view.xz * d + o.xz;
  gl_Position = vec4(image_coord, 0, 1);
}
