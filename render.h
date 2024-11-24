//
// Created by robin on 31/10/2024.
//

#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>

#include "triangle.h"
#include "vector.h"

// Declarar la variable global
extern vec2_t* projected_points;
extern face_t* sorted_faces;
extern triangle_t* visible_triangles;

void render_scene(vec3_t camera_pos, float fov_factor, bool show_faces, bool show_edges, bool show_vertices);


#endif //RENDER_H
