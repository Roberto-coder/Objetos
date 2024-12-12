#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include "triangle.h"
#include "vector.h"

// Declare global variables
extern vec2_t* projected_points;
extern triangle_t* visible_triangles;

extern float fov_factor;// Ajuste del factor de proyección
extern float aspect_ratio; // Relación de aspecto de la ventana


// Functions
void calculate_visible_faces(vec3_t camera_pos, float fov);
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool backface_culling_enabled);
uint32_t calculate_color_based_on_depth(float depth);
void draw_normal(vec3_t normal, vec3_t centroid, mat4_t world_matrix, mat4_t view_matrix, float aspect_ratio, float fov_factor, uint32_t color);
uint32_t calculate_depth_color(float depth, float z_min, float z_max);

#endif // RENDER_H