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
face_t* sorted_faces = NULL;

// Definir una estructura para la pila
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

// Función para inicializar y renderizar la escena
void render_scene(bool show_faces, bool show_edges, bool show_vertices) {
    // Crear y llenar una lista de triángulos con profundidad
    int num_faces = array_length(mesh.faces);
    triangle_t* triangles = malloc(sizeof(triangle_t) * num_faces);

    for (int i = 0; i < num_faces; i++) {
        face_t face = mesh.faces[i];
        triangles[i].points[0] = projected_points[face.a - 1];
        triangles[i].points[1] = projected_points[face.b - 1];
        triangles[i].points[2] = projected_points[face.c - 1];
        triangles[i].depth = calculate_triangle_depth(face);
    }

    // Ordenar triángulos por profundidad descendente
    qsort(triangles, num_faces, sizeof(triangle_t), compare_triangles);

    // Renderizar triángulos
    if (show_faces) {
        for (int i = 0; i < num_faces; i++) {
            face_t face = mesh.faces[i];
            triangle_t tri = triangles[i];
            vec2_t point_a = tri.points[0];
            vec2_t point_b = tri.points[1];
            vec2_t point_c = tri.points[2];

            draw_filled_triangle(
                point_a.x + window_width / 2, point_a.y + window_height / 2,
                point_b.x + window_width / 2, point_b.y + window_height / 2,
                point_c.x + window_width / 2, point_c.y + window_height / 2,
                face.color
            );
        }
    }

    if (show_edges) {
        for (int i = 0; i < num_faces; i++) {
            face_t face = mesh.faces[i];
            vec2_t point_a = projected_points[face.a - 1];
            vec2_t point_b = projected_points[face.b - 1];
            vec2_t point_c = projected_points[face.c - 1];

            draw_triangle(
                point_a.x + window_width / 2, point_a.y + window_height / 2,
                point_b.x + window_width / 2, point_b.y + window_height / 2,
                point_c.x + window_width / 2, point_c.y + window_height / 2,
                COLOR_ARISTAS
            );
        }
    }

    if (show_vertices) {
        int num_vertices = array_length(mesh.vertices);
        for (int i = 0; i < num_vertices; i++) {
            vec2_t point = projected_points[i];
            draw_vertex(point.x + window_width / 2, point.y + window_height / 2, COLOR_VERTICES);
        }
    }

    // Liberar memoria
    free(triangles);
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

