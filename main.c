#include <SDL.h>
#include <stdbool.h>
#include <GL/gl.h> // Include OpenGL header
#include <time.h>  // Include time.h for the time function
#include "display.h"
#include "mesh.h"
#include "vector.h"
#include "matrix.h"
#include "render.h"
#include "transform.h"

// Function prototypes
void setup(void);
void process_input(void);
void update(void);
void render(void);

// Global variables
bool is_running = false;
int previous_frame_time = 0;

bool show_faces = true;
bool show_edges = true;
bool show_vertices = true;
bool back_face_culling = true;

vec3_t object_rotation = {0, 0, 0};
vec3_t object_translation = {0, 0, 0}; // Adjust translation to bring the object closer to the camera
vec3_t camera_position = {-5, -5, -10}; // Adjust camera position
vec3_t camera_rotation = {0, 0, 0}; // Camera rotation

float fov_factor = 360; // Adjust projection factor
float aspect_ratio = 16.0f / 9.0f; // Window aspect ratio

mat4_t view_matrix; // Define view_matrix
mat4_t world_matrix; // Define world_matrix

int main(int argc, char* argv[]) {
    // Initialize SDL and window
    if (!(is_running = initialize_window())) {
        return -1;
    }

    // Enable Z-buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    setup();

    // Main loop
    while (is_running) {
        process_input();
        update();
        render();

        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }
        previous_frame_time = SDL_GetTicks();
    }

    // Free memory and close SDL
    if (projected_points != NULL) free(projected_points);
    destroy_window();
    return 0;
}

void setup(void) {
    srand(time(NULL)); // Initialize seed for generating random colors
    load_obj_file_data("../Objetos/icosphere.obj"); // Load OBJ model
    // Assign identity matrix to view_matrix and world_matrix
    view_matrix = mat4_identity();
    world_matrix = mat4_identity();
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            is_running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_c: back_face_culling = !back_face_culling; break; // Toggle culling
                case SDLK_w: object_translation.z += 1; break;
                case SDLK_s: object_translation.z -= 1; break;
                case SDLK_LEFT: camera_position.x += 1; break;
                case SDLK_RIGHT: camera_position.x -= 1; break;
                case SDLK_UP: camera_position.y += 1; break;
                case SDLK_DOWN: camera_position.y -= 1; break;
                case SDLK_q: camera_rotation.y -= 0.1; break; // Rotar cámara a la izquierda
                case SDLK_e: camera_rotation.y += 0.1; break; // Rotar cámara a la derecha
                case SDLK_r: camera_rotation.x -= 0.1; break; // Rotar cámara hacia arriba
                case SDLK_f: camera_rotation.x += 0.1; break; // Rotar cámara hacia abajo
            }
        }
    }
}

void update(void) {
    // Apply rotations
    rotate_object(&object_rotation, 0.01, 0.00, 0.00);

    // Create object transformation matrix
    world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(mat4_make_rotation_x(object_rotation.x), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_y(object_rotation.y), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_rotation_z(object_rotation.z), world_matrix);
    world_matrix = mat4_mul_mat4(mat4_make_translation(object_translation.x, object_translation.y, object_translation.z), world_matrix);

    // Create camera view matrix
    view_matrix = mat4_identity();
    view_matrix = mat4_mul_mat4(mat4_make_rotation_x(camera_rotation.x), view_matrix);
    view_matrix = mat4_mul_mat4(mat4_make_rotation_y(camera_rotation.y), view_matrix);
    view_matrix = mat4_mul_mat4(mat4_make_rotation_z(camera_rotation.z), view_matrix);
    view_matrix = mat4_mul_mat4(mat4_make_translation(-camera_position.x, -camera_position.y, -camera_position.z), view_matrix);

    // Recalculate visible faces
    calculate_visible_faces(camera_position, fov_factor, back_face_culling);
}

void render(void) {
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_scene(show_faces, show_edges, show_vertices, back_face_culling, aspect_ratio, fov_factor);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}