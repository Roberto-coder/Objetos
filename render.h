//
// Created by robin on 31/10/2024.
//

#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include "vector.h"

// Declarar la variable global
extern vec2_t* projected_points;

void render_scene(bool show_faces, bool show_edges, bool show_vertices);
void draw_vertex(int x, int y, uint32_t vertex_color);
void draw_point(int x, int y, uint32_t point_color);

#endif //RENDER_H
