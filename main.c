#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "mesh.h"
#include "vector.h"
#include "matrix.h"
#include "array.h"
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

// Declarar la matriz de transformación globalmente
mat4_t world_matrix;
vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 10}; // Aleja el objeto para que sea visible
float fov_factor = 720;

int main(int argc, char* argv[]) {
    // Inicializar la ventana SDL
    if (!(is_running = initialize_window())) {
        return -1;
    }

    // Configurar el entorno de renderizado
    setup();

    // Bucle principal
    while (is_running) {
        process_input();
        update();
        render();

        // Controlar el tiempo por frame
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }
        previous_frame_time = SDL_GetTicks();
    }

    // Liberar memoria asignada y destruir la ventana
    if (projected_points != NULL) {
        free(projected_points);
    }
    destroy_window();
    return 0;
}

void setup(void) {
    // Cargar un archivo OBJ específico
    load_obj_file_data("../Objetos/dona.obj");
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
    // Rotar el objeto
    rotate_object(&object_rotation, 0.01, 0.01, 0.01);

    // Matrices de transformación
    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(mat4_make_rotation_x(object_rotation.x), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_y(object_rotation.y), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_z(object_rotation.z), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_translation(object_translation.x, object_translation.y, object_translation.z), world_matrix);

    // Limpiar y asignar memoria para proyectar los vértices dinámicos
    if (projected_points != NULL) {
        free(projected_points);
    }
    projected_points = (vec2_t*)malloc(array_length(mesh.vertices) * sizeof(vec2_t));

    // Proyectar los vértices del objeto cargado
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec3_t vertex = mesh.vertices[i];
        vec4_t transformed_vertex = mat4_mul_vec4(world_matrix, vec4_from_vec3(vertex));
        vec3_t projected_vertex = vec3_from_vec4(transformed_vertex);
        projected_points[i] = project(projected_vertex, fov_factor);
    }
}

void render(void) {
    render_scene(show_faces, show_edges, show_vertices);
    // Actualizar el buffer y mostrar en pantalla
    render_color_buffer();
    clear_color_buffer(0xFF000000);  // Limpiar el buffer para el próximo frame
    SDL_RenderPresent(renderer);
}
