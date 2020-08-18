#version 330
// reference:
// 1. re-implement from PPTK project
// 2. https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders

// camera coordinate frame
uniform vec3 eye;
uniform vec3 right;
uniform vec3 left;
uniform vec3 up;
uniform vec3 view;
uniform float height;  // relative to floor

// image dimensions
uniform float h_lo;
uniform float h_hi;
uniform float r;  // right visual range based on vfov and distance
uniform float t;  // top visual range based on vfov and distance

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

//out vec2 floor_coord;
//out float distance;
out vec3 vertexPosition;       // vertex position for the fragment shader
out vec4 vColor;
//out vec3 near;
//out vec3 far;

void main() {
    // rescale muster rectangle from topleft (0,1), bottomdown(1,0) to (-r, h_hi),(+r, h_lo)
//    vec2 image_coord = position.xy * vec2(2.0 * r, h_hi - h_lo) + vec2(-r, h_lo);

    // transform muster rectangle from world to camera
    // use camera base to build
    // recall pinhole model, view->z, up->y, right->x
//    mat3 R = mat3(right, up, view);  //
//    vec3 p_cam = R * vec3(image_coord, -1);
//    near = p_cam * 0.1

//    p_world *= -height/p_world.z;
//    p_world *= -height/p_world.z;
//    vec3 p_camera = transpose(R) * p_world;

    //  gl_Position = vec4(p_camera.xy * vec2(1.0 / r, 1.0 / t), 0, );
    gl_Position = vec4(position.xy, 0.0, 1.0);
//    floor_coord = p_world.xy;
//    distance = length(p_world);
    vertexPosition = position.xyz;
    vColor = vec4(color, 1.0);
}
