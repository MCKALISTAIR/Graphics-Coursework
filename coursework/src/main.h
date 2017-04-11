#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

/*GLOBAL VARIABLES*/
// Effects
extern effect basicEff, normalMappingEff, shadows_eff, sky_eff, mask_eff, edge_eff, sepia_eff, motion_blur_eff, terrain_eff, waterEggs_eff, water_eff;

// Meshes
extern map<string, mesh> normalMapMeshes, basicMeshes, shadow_geom;
extern std::array<mesh, 5> hierarchicalMesh;
extern mesh skybox;

// Lights
extern directional_light dirLight;
extern vector<spot_light> spots;
extern point_light pointLight;

// Cameras
extern std::array<camera*, 2> cams;

// Camera indexes
extern int cameraIndex, targetCamera;

// Coordinates used for the Free camera
extern double cursor_x, cursor_y;

// Velocity used for the moving objects
extern double velocity;

// Textures and Normal maps
extern map<string, texture> textures, normal_maps;

// Shadow map
extern shadow_map shadowMap;

// Cube map
extern cubemap cube_map;

// Frame buffers
extern map<string, frame_buffer> frameBuffers;

// Post processing effects
extern map<string, int> effects;

extern geometry screen_quad;
extern frame_buffer blurFrames[2];
extern unsigned int current_frame;
extern texture terrainTex[4];
extern mesh terrainMesh;

extern const int eggsNumber;
extern std::array<vec3, 800>offsetArray;
extern mesh waterMesh;
extern float moveFactor;