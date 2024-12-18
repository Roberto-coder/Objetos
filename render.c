#include "render.h"
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#include "triangle.h"

// Colores de los elementos
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Rojo para caras
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Verde para aristas
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Azul para vértices

vec2_t* projected_points = NULL;
face_t* visible_faces = NULL;

void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling) {
    float aspect_ratio = (float)window_width / (float)window_height;

    if (visible_faces != NULL) {
        array_free(visible_faces);
    }
    visible_faces = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        // Transform vertices using the world matrix
        vertex_a = mat4_mul_vec3(world_matrix, vertex_a);
        vertex_b = mat4_mul_vec3(world_matrix, vertex_b);
        vertex_c = mat4_mul_vec3(world_matrix, vertex_c);

        // Transform vertices using the view matrix
        vertex_a = mat4_mul_vec3(view_matrix, vertex_a);
        vertex_b = mat4_mul_vec3(view_matrix, vertex_b);
        vertex_c = mat4_mul_vec3(view_matrix, vertex_c);

        // Calculate the normal of the face
        vec3_t normal = calculate_normal(vertex_a, vertex_b, vertex_c);

        face.depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Calculate depth

        // Check if the face is visible
        if (!back_face_culling || is_face_visible(normal, camera_pos, vertex_a)) {
            array_push(visible_faces, face);
        }
    }

    // Sort visible faces by depth
    qsort(visible_faces, array_length(visible_faces), sizeof(face_t), compare_faces_by_depth);
}

void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool back_face_culling, float aspect_ratio, float fov_factor, vec3_t camera_position) {
    calculate_visible_faces(camera_position, fov_factor, back_face_culling);
    face_t* faces_to_render = visible_faces;

    // Draw faces
    for (int i = 0; i < array_length(faces_to_render); i++) {
        face_t face = faces_to_render[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov_factor);

        if (show_faces) {
            draw_filled_triangle(
                projected_a.x, projected_a.y,
                projected_b.x, projected_b.y,
                projected_c.x, projected_c.y,
                face.color
            );
        }

        if (show_edges) {
            draw_triangle(
                projected_a.x, projected_a.y,
                projected_b.x, projected_b.y,
                projected_c.x, projected_c.y,
                COLOR_ARISTAS
            );
        }

        if (show_vertices) {
            draw_pixel(projected_a.x, projected_a.y, COLOR_VERTICES);
            draw_pixel(projected_b.x, projected_b.y, COLOR_VERTICES);
            draw_pixel(projected_c.x, projected_c.y, COLOR_VERTICES);
        }
    }
}