#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include "vector.h"

// Declare the global variables
extern vec2_t* projected_points;
extern mat4_t view_matrix;
extern mat4_t world_matrix;
extern bool backface_culling_enabled; // Declare as extern

// Function declarations
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling);
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool back_face_culling, float aspect_ratio, float fov_factor, vec3_t camera_position);

#endif // RENDER_H