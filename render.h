#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>

#include "triangle.h"
#include "vector.h"

typedef struct {
    vec3_t direction;
    vec3_t color; // Color de la luz
    float intensity; // Intensidad de la luz
} light_t;

// Declare the global variables
extern light_t light;
extern vec2_t* projected_points;
extern face_t* visible_faces;
extern const uint32_t COLOR_CARAS;
extern mat4_t view_matrix;
extern mat4_t world_matrix;

extern bool show_faces;
extern bool show_edges;
extern bool show_vertices;
extern bool apply_shading;
extern bool back_face_culling;

// Function declarations
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling);
uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);
void render_scene(float aspect_ratio, float fov_factor, vec3_t camera_position);

uint32_t linear_interpolation(uint32_t start, uint32_t end, float t);
void calculate_vertex_intensities(vec3_t camera_pos);
uint32_t phongIllumination(vec3_t normal, vec3_t lightDir, vec3_t viewDir, uint32_t ambient, uint32_t diffuse, uint32_t specular, float shininess);


#endif // RENDER_H