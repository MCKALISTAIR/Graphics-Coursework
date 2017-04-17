#version 440

// Requires direction.frag and fog.frag


#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations
float calculate_fog(in float fog_coord, in vec4 fog_colour, in float fog_start, in float fog_end, in float fog_density,
                    in int fog_type);

// Directional light for the scene

// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;
// Fog colour
uniform vec4 fog_colour;
// Fog start position
uniform float fog_start;
// Fog end position
uniform float fog_end;
// Fog density
uniform float fog_density;
// Fog type
uniform int fog_type;

// Incoming world position
layout(location = 0) in vec3 position;
// Camera space position
layout(location = 1) in vec4 CS_position;
// Transformed normal
layout(location = 2) in vec3 normal;
// Incoming tex_coord
layout(location = 3) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {

  // *********************************
  // Calculate fog coord
  // - convert from homogeneous
  // - ensure value is positive (we want the size of the value)
  float fog_coord = abs(CS_position.z / CS_position.w);

  // Calculate fog factor
  float fog_factor = calculate_fog(fog_coord, fog_colour, fog_start, fog_end, fog_density, fog_type);

  // Colour is mix between colour and fog colour based on factor
  colour = mix(colour,fog_colour,fog_factor);
  // *********************************
}