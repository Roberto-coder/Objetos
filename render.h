#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include "triangle.h"
#include "vector.h"

// Declare the global variables
extern vec2_t* projected_points;
extern mat4_t view_matrix;
extern mat4_t world_matrix; // Add this line

// Function declarations
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling);
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool back_face_culling, float aspect_ratio, float fov_factor);

#endif // RENDER_H