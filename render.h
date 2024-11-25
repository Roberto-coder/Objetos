#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include "triangle.h"
#include "vector.h"

// Declarar las variables globales
extern vec2_t* projected_points;
extern triangle_t* visible_triangles;

// Funciones
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling);
void render_scene(bool show_faces, bool show_edges, bool show_vertices);

#endif // RENDER_H