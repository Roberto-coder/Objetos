#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mesh.h"
#include "vector.h"
#include "transform.h"
#include "render.h"
#include "geometry.h"
#include "camera.h"

// Colores para los elementos
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Rojo para caras
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Verde para aristas
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Azul para vértices

// Definición de estructura para la pila
typedef enum { FACE, EDGE, VERTEX } render_type_t;
typedef struct {
    render_type_t* data;
    int top;
    int max_size;
} stack_t;

// Inicializar la pila
void init_stack(stack_t* stack, int max_size) {
    stack->data = (render_type_t*)malloc(max_size * sizeof(render_type_t));
    stack->top = -1;
    stack->max_size = max_size;
}

// Funciones para operar la pila
void push(stack_t* stack, render_type_t item) {
    if (stack->top < stack->max_size - 1) {
        stack->data[++stack->top] = item;
    }
}

bool is_empty(stack_t* stack) {
    return stack->top == -1;
}

// Renderizar la escena con las opciones activadas
void render_scene(bool show_faces, bool show_edges, bool show_vertices, vec3_t camera_position, float fov_factor, vec2_t* projected_points) {
    // Calcular la matriz de vista
    mat4_t view_matrix = mat4_make_look_at(camera_position, (vec3_t){0, 0, 0}, (vec3_t){0, 1, 0});

    vec3_t transformed_vertices[MAX_VERTICES];
    for (int i = 0; i < vertex_count; i++) {
        transformed_vertices[i] = transform_vertices_to_camera_space(view_matrix, vertices[i]);
    }

    // Filtrar triángulos visibles
    triangle_t visible_triangles[MAX_TRIANGLES];
    int visible_triangle_count = 0;

    for (int i = 0; i < triangle_count; i++) {
        triangle_t* tri = &triangles[i];

        // Transformar los vértices al espacio de la cámara
        // Transformar cada vértice individualmente
        tri->vertices[0] = transform_vertices_to_camera_space(view_matrix, tri->vertices[0]);
        tri->vertices[1] = transform_vertices_to_camera_space(view_matrix, tri->vertices[1]);
        tri->vertices[2] = transform_vertices_to_camera_space(view_matrix, tri->vertices[2]);


        // Verificar visibilidad
        if (is_visible(tri->vertices[0], tri->vertices[1], tri->vertices[2], camera_position)) {
            // Calcular profundidad
            tri->depth = calculate_triangle_depth(tri->vertices[0], tri->vertices[1], tri->vertices[2]);

            visible_triangles[visible_triangle_count++] = *tri;
        }
    }

    // Ordenar triángulos por profundidad
    sort_triangles_by_depth(visible_triangles, visible_triangle_count);

    // Dibujar según configuración
    for (int i = 0; i < visible_triangle_count; i++) {
        triangle_t tri = visible_triangles[i];
        vec2_t projected_points[3];

        for (int j = 0; j < 3; j++) {
            vec3_t vertex_in_camera_space = transform_vertices_to_camera_space(view_matrix, tri.vertices[j]);
            projected_points[j] = project(vertex_in_camera_space, fov_factor);
        }

        if (show_faces) {
            draw_filled_triangle(
                projected_points[0].x + window_width / 2, projected_points[0].y + window_height / 2,
                projected_points[1].x + window_width / 2, projected_points[1].y + window_height / 2,
                projected_points[2].x + window_width / 2, projected_points[2].y + window_height / 2,
                COLOR_CARAS
            );
        }

        if (show_edges) {
            draw_triangle(
                projected_points[0].x + window_width / 2, projected_points[0].y + window_height / 2,
                projected_points[1].x + window_width / 2, projected_points[1].y + window_height / 2,
                projected_points[2].x + window_width / 2, projected_points[2].y + window_height / 2,
                COLOR_ARISTAS
            );
        }

        if (show_vertices) {
            for (int j = 0; j < 3; j++) {
                draw_vertex(
                    projected_points[j].x + window_width / 2, projected_points[j].y + window_height / 2,
                    COLOR_VERTICES
                );
            }
        }
    }
}

// Dibujar un vértice
void draw_vertex(int x, int y, uint32_t vertex_color) {
    int thickness = 2; // Grosor del vértice
    for (int offset_y = -thickness; offset_y <= thickness; offset_y++) {
        for (int offset_x = -thickness; offset_x <= thickness; offset_x++) {
            int draw_x = x + offset_x;
            int draw_y = y + offset_y;

            if (draw_x >= 0 && draw_x < window_width && draw_y >= 0 && draw_y < window_height) {
                color_buffer[(window_width * draw_y) + draw_x] = vertex_color;
            }
        }
    }
}

// Dibujar un punto
void draw_point(int x, int y, uint32_t point_color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = point_color;
    }
}
