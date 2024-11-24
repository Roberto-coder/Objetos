#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "mesh.h"
#include "vector.h"
#include "matrix.h"
#include "array.h"
#include "camera.h"
#include "render.h"
#include "transform.h"

// Prototipo de funciones
void setup(void);
void process_input(void);
void update(void);
void render(void);

// Variables globales
bool is_running = false;
int previous_frame_time = 0;

// Variables de control de visualización
bool show_faces = true;
bool show_edges = true;
bool show_vertices = true;

// Declarar matrices y transformaciones globales
mat4_t world_matrix;
mat4_t view_matrix; // Matriz de vista
vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 10}; // Aleja el objeto para que sea visible
float fov_factor = 720;
camera_t camera;

// Declaración de puntos proyectados y vértices
vec2_t* projected_points = NULL;
vec3_t vertices[MAX_VERTICES]; // Arreglo de vértices, ahora definido de acuerdo con tu declaración global en triangle.h
int vertex_count = 0;  // Contador de vértices

int main(int argc, char* argv[]) {
    // Initialize SDL window
    if (!(is_running = initialize_window())) {
        return -1;
    }

    // Allocate memory for projected_points
    projected_points = (vec2_t*)malloc(MAX_TRIANGLES * 3 * sizeof(vec2_t));
    // Ensure allocation was successful
    if (projected_points == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Failed to allocate memory for projected_points\n");
        destroy_window();
        return -1;
    }

    // Set up the rendering environment
    setup();

    // Main loop
    while (is_running) {
        process_input();
        update();
        render();

        // Frame timing control
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }
        previous_frame_time = SDL_GetTicks();
    }

    // Free allocated memory and destroy SDL window
    if (projected_points != NULL) {
        free(projected_points);
    }
    destroy_window();
    return 0;
}

void setup(void) {
    // Inicializar la cámara con posición, objetivo y vector hacia arriba
    camera = camera_create(
    (vec3_t){0, 0, 0},  // Posición inicial de la cámara
    (vec3_t){0, 0, 0},   // Cambiar el objetivo al origen
    (vec3_t){0, 1, 0}    // Vector hacia arriba
);


    // Cargar el archivo OBJ
    load_obj_file_data("../Objetos/cubo3.obj");

    // Asignar los vértices desde la malla cargada
    vertex_count = array_length(mesh.vertices);
    for (int i = 0; i < vertex_count; i++) {
        vertices[i] = mesh.vertices[i];
    }
}


void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            is_running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_f: show_faces = !show_faces; break;
                case SDLK_l: show_edges = !show_edges; break;
                case SDLK_v: show_vertices = !show_vertices; break;
            }
        }
    }
}

void update(void) {
    // Actualizar la matriz de vista con base en la cámara
    mat4_t view_matrix = camera_get_view_matrix(camera);

    // Transformar y proyectar los vértices
    if (projected_points != NULL) {
        free(projected_points);
    }
    projected_points = (vec2_t*)malloc(vertex_count * sizeof(vec2_t));

    for (int i = 0; i < vertex_count; i++) {
        vec3_t transformed_vertex = transform_vertices_to_camera_space(view_matrix, vertices[i]);
        projected_points[i] = project(transformed_vertex, fov_factor);
    }
}

// Función para imprimir una matriz 4x4
void print_matrix(mat4_t matrix) {
    printf("Matriz 4x4:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.3f ", matrix.m[i][j]);
        }
        printf("\n");
    }
}

void render(void) {
    render_scene(show_faces, show_edges, show_vertices, camera, fov_factor, projected_points);
    // Actualizar el buffer y mostrar en pantalla
    render_color_buffer();
    clear_color_buffer(0xFF000000);  // Limpiar el buffer para el próximo frame
    SDL_RenderPresent(renderer);
}
