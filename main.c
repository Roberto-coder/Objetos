#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "mesh.h"
#include "vector.h"
#include "matrix.h"
#include "array.h"

// Prototipo de funciones
void setup(void);
void process_input(void);
void update(void);
void render(void);

// Variables globales
bool is_running = false;
int previous_frame_time = 0;
// Declarar la matriz de transformación globalmente
mat4_t world_matrix;
vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 10}; // Aleja el objeto para que sea visible
float fov_factor = 720;
vec2_t projected_points[N_CUBE_VERTICES];

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

    destroy_window();
    return 0;
}

void setup(void) {
    // Cargar datos del cubo directamente
    load_cube_mesh_data();
    //
    //load_obj_file_data("cubo.obj");
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            is_running = false;
        }
    }
}

void update(void) {
    // Incrementar la rotación del objeto
    object_rotation.x += 0.01;
    object_rotation.y += 0.01;
    object_rotation.z += 0.01;

    // Matrices de transformación
    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(mat4_make_rotation_x(object_rotation.x), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_y(object_rotation.y), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_z(object_rotation.z), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_translation(object_translation.x, object_translation.y, object_translation.z), world_matrix);

    // Proyectar los vértices del objeto cargado
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec3_t vertex = mesh.vertices[i];
        vec4_t transformed_vertex = mat4_mul_vec4(world_matrix, vec4_from_vec3(vertex));
        vec3_t projected_vertex = vec3_from_vec4(transformed_vertex);
        projected_points[i] = project(projected_vertex, fov_factor);
    }
}
void render(void) {
    // Dibujar los triángulos del cubo
    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec2_t point_a = projected_points[face.a - 1];
        vec2_t point_b = projected_points[face.b - 1];
        vec2_t point_c = projected_points[face.c - 1];

        draw_triangle(
            point_a.x + window_width / 2, point_a.y + window_height / 2,
            point_b.x + window_width / 2, point_b.y + window_height / 2,
            point_c.x + window_width / 2, point_c.y + window_height / 2,
            face.color
        );
    }

    // Actualizar el buffer y mostrar en pantalla
    render_color_buffer();
    clear_color_buffer(0xFF000000);  // Limpiar el buffer para el próximo frame
    SDL_RenderPresent(renderer);
}
