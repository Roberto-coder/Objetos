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

void render_scene(bool show_faces, bool show_edges, bool show_vertices) {
    stack_t render_stack;
    init_stack(&render_stack, 3);

    // Chequear qué elementos se deben mostrar y añadirlos a la pila
    if (show_faces) {
        push(&render_stack, FACE);
    }
    if (show_edges) {
        push(&render_stack, EDGE);
    }
    if (show_vertices) {
        push(&render_stack, VERTEX);
    }

    // Si la pila está vacía, forzar que al menos un tipo esté activo
    if (is_empty(&render_stack)) {
        push(&render_stack, EDGE);  // Por ejemplo, aseguramos dibujar al menos aristas
    }

    // Renderizar elementos según lo que haya en la pila
    for (int i = 0; i <= render_stack.top; i++) {
        switch (render_stack.data[i]) {
            case FACE:
                for (int j = 0; j < array_length(mesh.faces); j++) {
                    face_t face = mesh.faces[j];
                    vec2_t point_a = projected_points[face.a - 1];
                    vec2_t point_b = projected_points[face.b - 1];
                    vec2_t point_c = projected_points[face.c - 1];
                    draw_filled_triangle(
                        point_a.x + window_width / 2, point_a.y + window_height / 2,
                        point_b.x + window_width / 2, point_b.y + window_height / 2,
                        point_c.x + window_width / 2, point_c.y + window_height / 2,
                        COLOR_CARAS
                    );
                }
                break;
            case EDGE:
                for (int j = 0; j < array_length(mesh.faces); j++) {
                    face_t face = mesh.faces[j];
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
                break;
            case VERTEX:
                for (int j = 0; j < array_length(mesh.vertices); j++) {
                    vec2_t point = projected_points[j];
                    draw_vertex(point.x + window_width / 2, point.y + window_height / 2, COLOR_VERTICES);
                }
                break;
        }
    }

    // Liberar memoria de la pila
    free(render_stack.data);
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
