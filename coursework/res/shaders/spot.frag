#version 440

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate direction to the light
  vec3 lightDirection = normalize(spot.position - vertex_position);
  // Calculate distance to light
  float distance = distance(spot.position, vertex_position);
  // Calculate attenuation value
  float attFactor = spot.constant + (spot.linear * distance) + (spot.quadratic * distance * distance);
  // Calculate spot light intensity
  float intensity = pow(max(dot((-1 * spot.direction), lightDirection), 0.0f), spot.power);
  // Calculate light colour
  vec4 lightColour = spot.light_colour * (intensity / attFactor);
  // Calculate view direction
  vec3 viewDirection = normalize(eye_pos - vertex_position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient

  float k1 = max(dot(transformed_normal, lightDirection), 0.0f);
  vec4 diffuse = k1 * (mat.diffuse_reflection * lightColour);
  vec3 H = normalize(lightDirection + viewDirection);
  float k2 = pow(max(dot(transformed_normal, H), 0.0f), mat.shininess);
  vec4 specular = k2 * (mat.specular_reflection * lightColour);
  vec4 texSample = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  colour.a = 1.0f;
  colour = primary * texSample + specular;
  // *********************************
}