<<<<<<< Updated upstream
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
=======

#include "render.h"

#include <stdio.h>

>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
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
=======
vec2_t* projected_points = NULL;
face_t* visible_faces = NULL;

// render.c
void render_scene(vec3_t camera_pos, float fov, bool show_faces, bool show_edges, bool show_vertices, int max_faces) {
    float aspect_ratio = (float)window_width / (float)window_height;

    if (visible_faces != NULL) {
        array_free(visible_faces);
    }
    visible_faces = NULL;

    int total_faces = array_length(mesh.faces);
    int renderable_faces = max_faces < total_faces ? max_faces : total_faces;

    for (int i = 0; i < renderable_faces; i++) {
>>>>>>> Stashed changes
        face_t face = mesh.faces[i];

        vec2_t points[3] = {
            projected_points[face.a],
            projected_points[face.b],
            projected_points[face.c]
        };

<<<<<<< Updated upstream
        // Usar la función para rellenar triángulos
        draw_filled_triangle(
            points[0].x, points[0].y,
            points[1].x, points[1].y,
            points[2].x, points[2].y,
            face.color // Color asociado al triángulo
=======
        vec3_t normal = calculate_normal(vertex_a, vertex_b, vertex_c);

        if (is_face_visible(normal, camera_pos, vertex_a)) {
            vec3_t transformed_a = mat4_mul_vec3(world_matrix, vertex_a);
            vec3_t transformed_b = mat4_mul_vec3(world_matrix, vertex_b);
            vec3_t transformed_c = mat4_mul_vec3(world_matrix, vertex_c);

            vec3_t camera_vertex_a = vec3_sub(transformed_a, camera_pos);
            vec3_t camera_vertex_b = vec3_sub(transformed_b, camera_pos);
            vec3_t camera_vertex_c = vec3_sub(transformed_c, camera_pos);

            vec2_t projected_a = project(camera_vertex_a, fov, aspect_ratio);
            vec2_t projected_b = project(camera_vertex_b, fov, aspect_ratio);
            vec2_t projected_c = project(camera_vertex_c, fov, aspect_ratio);

            float face_depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f;
            face.depth = face_depth;

            array_push(visible_faces, face);
        }
    }

    qsort(visible_faces, array_length(visible_faces), sizeof(face_t), compare_triangles_by_depth);

    for (int i = 0; i < array_length(visible_faces); i++) {
        face_t face = visible_faces[i];
        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        vec3_t transformed_a = mat4_mul_vec3(world_matrix, vertex_a);
        vec3_t transformed_b = mat4_mul_vec3(world_matrix, vertex_b);
        vec3_t transformed_c = mat4_mul_vec3(world_matrix, vertex_c);

        vec3_t camera_vertex_a = vec3_sub(transformed_a, camera_pos);
        vec3_t camera_vertex_b = vec3_sub(transformed_b, camera_pos);
        vec3_t camera_vertex_c = vec3_sub(transformed_c, camera_pos);

        vec2_t projected_a = project(camera_vertex_a, fov, aspect_ratio);
        vec2_t projected_b = project(camera_vertex_b, fov, aspect_ratio);
        vec2_t projected_c = project(camera_vertex_c, fov, aspect_ratio);

        draw_filled_triangle(
            projected_a.x, projected_a.y,
            projected_b.x, projected_b.y,
            projected_c.x, projected_c.y,
            face.color
>>>>>>> Stashed changes
        );
    }
}

<<<<<<< Updated upstream
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

=======
>>>>>>> Stashed changes
