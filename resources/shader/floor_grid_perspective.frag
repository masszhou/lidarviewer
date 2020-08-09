//#version 330
//uniform vec3 eye;
//uniform vec3 right;
//uniform vec3 up;
//uniform vec3 view;
//uniform float height;

//uniform vec4 line_color;
//uniform vec4 floor_color;
//uniform float cell_size;
//uniform float line_weight;
//uniform float line_width;

//uniform float max_dist_in_focus;
//uniform float max_dist_visible;

//in vec2 floor_coord;
//in float distance;

//out vec4 fColor;

//float compute_weight(vec3 n, vec2 image_coord) {
//  vec3 line = transpose(mat3(right, up, view))*n;
//  line /= length(line.xy);
//  float eps = -abs(dot(vec3(image_coord, -1), line));
//  return (eps+line_width) / line_width;
//}
//void main() {
//  vec2 cell_idx = floor((floor_coord + eye.xy) / cell_size);
//  vec2 cell_min = cell_idx * cell_size - eye.xy;
//  vec2 cell_max = cell_min + cell_size;
//  float i = mod(cell_idx.x, 10.0);
//  float j = mod(cell_idx.y, 10.0);
//  float x_min_weight = i == 0.0 ? 1.0 : line_weight;
//  float x_max_weight = i == 9.0 ? 1.0 : line_weight;
//  float y_min_weight = j == 0.0 ? 1.0 : line_weight;
//  float y_max_weight = j == 9.0 ? 1.0 : line_weight;

//  vec3 temp = transpose(mat3(right, up, view)) * vec3(floor_coord, -height);
//  vec2 image_coord = -temp.xy / temp.z;
//  float weight = 0.0;
//  weight = max(weight, x_min_weight * compute_weight(vec3(height, 0, cell_min.x), image_coord));
//  weight = max(weight, x_max_weight * compute_weight(vec3(height, 0, cell_max.x), image_coord));
//  weight = max(weight, y_min_weight * compute_weight(vec3(0, height, cell_min.y), image_coord));
//  weight = max(weight, y_max_weight * compute_weight(vec3(0, height, cell_max.y), image_coord));
//  weight *= 0.7;

//  float blur_weight = clamp((max_dist_visible - distance) / (max_dist_visible - max_dist_in_focus), 0.0, 1.0);
//  vec4 c = line_color * weight+floor_color*(1.0 - weight);
//  fColor = vec4(c.xyz, c.w * blur_weight);
//}

#version 330
in vec4 vColor;
in vec3 vertexPosition;

out vec4 fColor;
void main(){
    float c = (int(round(vertexPosition.x * 10.0)) + int(round(vertexPosition.y * 10.0)) ) % 2;

    fColor = vec4(vec3(c/2.0 + 0.3), 1);
}
