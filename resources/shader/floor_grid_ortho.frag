#version 330
// taken from PPTK project
uniform float eps_x;
uniform float eps_y;
uniform float cell_size;    // for minor grid cells
uniform float line_weight;  // for minor grid lines
uniform vec4 floor_color;
uniform vec4 line_color;

in vec2 floor_coord;

out vec4 fColor;

void main() {
  vec2 cell_idx = floor(floor_coord / cell_size);
  vec2 cell_min = cell_idx * cell_size;
  vec2 cell_max = cell_min + cell_size;
  vec2 ij = mod(cell_idx, 10.0);
  float x_min_weight = ij.x == 0.0 ? 1.0 : line_weight;
  float x_max_weight = ij.x == 9.0 ? 1.0 : line_weight;
  float y_min_weight = ij.y == 0.0 ? 1.0 : line_weight;
  float y_max_weight = ij.y == 9.0 ? 1.0 : line_weight;

  float weight = 0.0;
  weight = max(weight, x_min_weight * (1.0 - (floor_coord.x - cell_min.x) / eps_x));
  weight = max(weight, y_min_weight * (1.0 - (floor_coord.y - cell_min.y) / eps_y));
  weight = max(weight, x_max_weight * (1.0 + (floor_coord.x - cell_max.x) / eps_x));
  weight = max(weight, y_max_weight * (1.0 + (floor_coord.y - cell_max.y) / eps_y));
  weight *= 0.7;

  fColor = floor_color * (1.0 - weight) + line_color * (weight);
}
