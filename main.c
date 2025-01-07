#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
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
bool show_edges = false;
bool show_vertices = false;
bool back_face_culling = false;
bool apply_shading = true;

vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 0};
vec3_t camera_position = {0, 0, -5}; // Alejar la cámara

// Definición de world_matrix y view_matrix
mat4_t world_matrix;
mat4_t view_matrix;
mat4_t projection_matrix;

float fov_factor = 720;
float aspect_ratio = 16.0f / 9.0f;

int main(int argc, char* argv[]) {
    // Inicializar SDL y otros componentes
    if (!initialize_window()) {
        return -1;
    }

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}

void setup(void) {
    // Configuración inicial
    is_running = true;
    world_matrix = mat4_identity();
    view_matrix = mat4_identity();
    projection_matrix = mat4_make_perspective(M_PI / 3, aspect_ratio, 0.1f, 100.0f);

    // Inicializar la posición del objeto en el eje -z
    object_translation.z = -7.0f;

    // Cargar datos del archivo .obj
    load_obj_file_data("../Objetos/cubo3.obj");
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        is_running = false;
                        break;
                    case SDLK_UP:
                        object_translation.z += 0.1f;
                        break;
                    case SDLK_DOWN:
                        object_translation.z -= 0.1f;
                        break;
                    case SDLK_f:
                        show_faces = !show_faces;
                        break;
                    case SDLK_a:
                        show_edges = !show_edges;
                        break;
                    case SDLK_v:
                        show_vertices = !show_vertices;
                        break;
                    case SDLK_c:
                        back_face_culling = !back_face_culling;
                        break;
                    case SDLK_s:
                        apply_shading = !apply_shading;
                        break;
                }
                break;
        }
    }
}

void update(void) {
    // Aplicar rotaciones con incrementos menores
    rotate_object(&object_rotation, 0.001, 0.001, 0.001);

    // Crear matriz de transformación
    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(mat4_make_rotation_x(object_rotation.x), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_y(object_rotation.y), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_z(object_rotation.z), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_translation(object_translation.x, object_translation.y, object_translation.z), world_matrix);

    // Actualizar la matriz de vista en función de la posición de la cámara
    view_matrix = mat4_look_at(camera_position, (vec3_t){0, 0, 0}, (vec3_t){0, 1, 0});

    // Calcular las caras visibles
    calculate_visible_faces(camera_position, fov_factor, back_face_culling);
}

void render(void) {
    render_scene(aspect_ratio, fov_factor, camera_position);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}