#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "mesh.h"
#include "vector.h"
#include "matrix.h"
#include "render.h"
#include "transform.h"

// Prototipos de funciones
void setup(void);
void process_input(void);
void update(void);
void render(void);

// Variables globales
bool is_running = false;
int previous_frame_time = 0;

bool show_faces = true;
bool show_edges = true;
bool show_vertices = true;
bool back_face_culling = true;

vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 0}; // Ajuste de la traslación para que el objeto esté más cerca de la cámara
vec3_t camera_position = {0, 0, -5}; // Ajuste de la posición de la cámara

// Definición de world_matrix
mat4_t world_matrix = {
    .m = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }
};

float fov_factor = 720; // Ajuste del factor de proyección
float aspect_ratio = 16.0f / 9.0f; // Relación de aspecto de la ventana

int main(int argc, char* argv[]) {
    // Inicializar SDL y ventana
    if (!(is_running = initialize_window())) {
        return -1;
    }

    setup();

    // Bucle principal
    while (is_running) {
        process_input();
        update();
        calculate_visible_faces(camera_position, fov_factor, back_face_culling); // Recalcular las caras visibles
        render();

        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }
        previous_frame_time = SDL_GetTicks();
    }

    // Liberar memoria y cerrar SDL
    if (projected_points != NULL) free(projected_points);
    destroy_window();
    return 0;
}

void setup(void) {
    load_obj_file_data("../Objetos/dona.obj"); // Cargar modelo OBJ
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
                case SDLK_c: back_face_culling = !back_face_culling; break; // Alternar el culling
                case SDLK_w: object_translation.z += 1; break;
                case SDLK_s: object_translation.z -= 1; break;
                case SDLK_LEFT: camera_position.x += 1; break;
                case SDLK_RIGHT: camera_position.x -= 1; break;
                case SDLK_UP: camera_position.y += 1; break;
                case SDLK_DOWN: camera_position.y -= 1; break;

            }
        }
    }
}

void update(void) {
    // Aplicar rotaciones
    rotate_object(&object_rotation, 0.01, 0.01, 0.01);

    // Crear matriz de transformación
    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(mat4_make_rotation_x(object_rotation.x), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_y(object_rotation.y), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_z(object_rotation.z), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_translation(object_translation.x, object_translation.y, object_translation.z), world_matrix);

    // Recalcular las caras visibles
    calculate_visible_faces(camera_position, fov_factor, back_face_culling);
}

void render(void) {
    render_scene(show_faces, show_edges, show_vertices);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}