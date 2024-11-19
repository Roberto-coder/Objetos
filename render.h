//
// Created by robin on 31/10/2024.
//

#ifndef RENDER_H
#define RENDER_H


#include <stdbool.h>
#include <stdint.h>
#include "vector.h"

// Máximo número de vértices que se pueden procesar
#define MAX_VERTICES 1000

// Máximo número de triángulos que se pueden procesar
#define MAX_TRIANGLES 500

// Declarar la variable global
extern vec2_t* projected_points;

// Número actual de vértices procesados
extern int vertex_count;

// Número actual de triángulos procesados
extern int triangle_count;

void render_scene(bool show_faces, bool show_edges, bool show_vertices, vec3_t camera_position, float fov_factor, vec2_t* projected_points);

void draw_vertex(int x, int y, uint32_t vertex_color);
void draw_point(int x, int y, uint32_t point_color);

#endif //RENDER_H
