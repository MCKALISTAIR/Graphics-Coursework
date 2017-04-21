#version 430 core

// Incoming texture containing frame information
uniform sampler2D tex;

// Our colour filter - calculates colour intensity
const vec3 intensity = vec3(0.8, 0.8, 0.8);

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  /// ************************************
  // Sample texture colour
  vec4 tex_colour = texture(tex, tex_coord);
  // Calculate gray value
  float i = dot(intensity, vec3(tex_colour).xyz);
  colour = vec4(i,i,i,1.0);
  colour += vec4(1.0 ,0.0, 0.0, 1.0);
colour.a = 1.0;
  // *********************************-
}