#include <stdio.h>
#include <stdbool.h>
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "render.h"

// Colores para los elementos
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Rojo para caras
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Verde para aristas
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Azul para vértices

vec2_t* projected_points = NULL;

void render_scene(bool show_faces, bool show_edges, bool show_vertices) {
    // Renderizar las caras si show_faces es verdadero
    if (show_faces) {
        for (int i = 0; i < array_length(mesh.faces); i++) {
            face_t face = mesh.faces[i];

            // Obtener los puntos proyectados de los vértices de la cara
            vec2_t point_a = projected_points[face.a - 1];
            vec2_t point_b = projected_points[face.b - 1];
            vec2_t point_c = projected_points[face.c - 1];

            // Rellenar el triángulo de la cara con color
            draw_filled_triangle(
                point_a.x + window_width / 2, point_a.y + window_height / 2,
                point_b.x + window_width / 2, point_b.y + window_height / 2,
                point_c.x + window_width / 2, point_c.y + window_height / 2,
                COLOR_CARAS
            );
        }
    }

    // Renderizar las aristas si show_edges es verdadero
    if (show_edges) {
        for (int i = 0; i < array_length(mesh.faces); i++) {
            face_t face = mesh.faces[i];

            // Obtener los puntos proyectados de los vértices de la cara
            vec2_t point_a = projected_points[face.a - 1];
            vec2_t point_b = projected_points[face.b - 1];
            vec2_t point_c = projected_points[face.c - 1];

            // Dibujar las líneas para cada arista del triángulo
            draw_triangle(
                point_a.x + window_width / 2, point_a.y + window_height / 2,
                point_b.x + window_width / 2, point_b.y + window_height / 2,
                point_c.x + window_width / 2, point_c.y + window_height / 2,
                COLOR_ARISTAS
            );
        }
    }

    // Renderizar los vértices si show_vertices es verdadero
    if (show_vertices) {
        for (int i = 0; i < array_length(mesh.vertices); i++) {
            vec2_t point = projected_points[i];

            // Dibujar el punto para el vértice
            draw_vertex(
                point.x + window_width / 2, point.y + window_height / 2,
                COLOR_VERTICES
            );
        }
    }
}

void draw_vertex(int x, int y, uint32_t vertex_color) {
    int thickness = 2; // Grosor ajustable para el tamaño del vértice
    for (int offset_y = -thickness; offset_y <= thickness; offset_y++) {
        for (int offset_x = -thickness; offset_x <= thickness; offset_x++) {
            int draw_x = x + offset_x;
            int draw_y = y + offset_y;

            // Verificar que cada punto esté dentro de los límites de la ventana
            if (draw_x >= 0 && draw_x < window_width && draw_y >= 0 && draw_y < window_height) {
                color_buffer[(window_width * draw_y) + draw_x] = vertex_color;
            }
        }
    }
}

void draw_point(int x, int y, uint32_t point_color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = point_color;
    }
}
