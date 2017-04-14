#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

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

// Point lights being used in the scene
uniform point_light points[4];
// Spot lights being used in the scene
uniform spot_light spots[5];
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

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
  float dist = distance(point.position, position);
  // Calculate attenuation factor
  float attFactor = 1.0f / (point.quadratic * pow(dist, 2.0f) + (point.linear * dist) + point.constant);
  // Calculate light colour
  vec4 lightColour = point.light_colour * attFactor;
  // Calculate light dir
  vec3 lightDirection = normalize(point.position - position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k1 = max(dot(transformed_normal, lightDirection), 0.0f);
  vec4 diffuse = k1 * (mat.diffuse_reflection * lightColour);
  vec3 H = normalize(lightDirection + view_dir);
  float k2 = pow(max(dot(normal, H), 0.0f), mat.shininess);
  vec4 specular = k2 * (mat.specular_reflection * lightColour);
  vec4 primary = mat.emissive + diffuse;
  colour = primary * tex_colour + specular;
  colour.a = 1.0f;
  // *********************************
  return colour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 lightDirection = normalize(spot.position - position);
  // Calculate distance to light
  float dist = distance(spot.position, position);
  // Calculate attenuation value
  float attFactor = 1.0f / (spot.quadratic * pow(dist, 2.0f) + (spot.linear * dist) + spot.constant);
  // Calculate spot light intensity
  float intensity = pow(max(dot((-1 * spot.direction), lightDirection), 0.0f), spot.power);
  // Calculate light colour
  vec4 lightColour = spot.light_colour * attFactor;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k1 = max(dot(normal, lightDirection), 0.0f);
  vec4 diffuse = k1 * (mat.diffuse_reflection * lightColour);
  vec3 H = normalize(lightDirection + view_dir);
  float k2 = pow(max(dot(normal, H), 0.0f), mat.shininess);
  vec4 specular = k2 * (mat.specular_reflection * lightColour);
  vec4 primary = mat.emissive + diffuse;
  colour = primary * tex_colour + specular;
  colour.a = 1.0f;
  // *********************************
  return colour;
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
  vec3 viewDirection = normalize(eye_pos - vertex_position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Sum point lights
  for (int i = 0; i < 1; ++i) 
  {
	colour += calculate_point(points[0], mat, vertex_position, transformed_normal, viewDirection, tex_colour);
  }
  // Sum spot lights
  //for (int i = 0; i < 1; ++i) 
  {
//	colour += calculate_spot(spots[0], mat, vertex_position, transformed_normal, viewDirection, tex_colour);
  }
  colour.a = 1.0f;
  // *********************************
}