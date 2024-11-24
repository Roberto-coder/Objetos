#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mesh.h"
#include "vector.h"
#include "transform.h"
#include "render.h"
#include "geometry.h"
#include "camera.h"
#include "display.h"

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
#include "camera.h"

void render_scene(bool show_faces, bool show_edges, bool show_vertices, camera_t camera, float fov_factor, vec2_t* projected_points) {
    if (projected_points == NULL) {
        fprintf(stderr, "Error: projected_points es nulo en render_scene\n");
        return;
    }

    // Renderizar las caras, bordes y vértices según las banderas
    if (show_faces) {
        render_faces(projected_points);
    }
    if (show_edges) {
        render_edges(projected_points);
    }
    if (show_vertices) {
        render_vertices(projected_points);
    }
}


void render_faces(vec2_t* projected_points) {
    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec2_t points[3] = {
            projected_points[face.a],
            projected_points[face.b],
            projected_points[face.c]
        };

        // Usar la función para rellenar triángulos
        draw_filled_triangle(
            points[0].x, points[0].y,
            points[1].x, points[1].y,
            points[2].x, points[2].y,
            face.color // Color asociado al triángulo
        );
    }
}

void render_edges(vec2_t* projected_points) {
    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec2_t points[3] = {
            projected_points[face.a],
            projected_points[face.b],
            projected_points[face.c]
        };

        // Dibujar las líneas de los bordes del triángulo
        draw_line(points[0].x, points[0].y, points[1].x, points[1].y, 0xFF0000FF); // Color azul
        draw_line(points[1].x, points[1].y, points[2].x, points[2].y, 0xFF0000FF);
        draw_line(points[2].x, points[2].y, points[0].x, points[0].y, 0xFF0000FF);
    }
}

void render_vertices(vec2_t* projected_points) {
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec2_t point = projected_points[i];

        // Dibujar un punto en la posición proyectada
        draw_pixel(point.x, point.y, 0xFFFF00); // Color amarillo
    }
}

