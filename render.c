#include "render.h"
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"

// Colores de los elementos
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Rojo para caras
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Verde para aristas
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Azul para vértices

vec2_t* projected_points = NULL;
triangle_t* visible_triangles = NULL;

void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling) {
    float aspect_ratio = (float)window_width / (float)window_height;

    if (visible_triangles != NULL) {
        array_free(visible_triangles);
    }
    visible_triangles = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        // Calculate the normal of the face
        vec3_t normal = calculate_normal(vertex_a, vertex_b, vertex_c);

        // Check if the face is visible
        if (!back_face_culling || is_face_visible(normal, camera_pos, vertex_a)) {
            // Project the vertices
            vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov);
            vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov);
            vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov);

            face.depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Calculate depth

            triangle_t triangle = {
                .points = {projected_a, projected_b, projected_c},
                .depth = face.depth
            };

            array_push(visible_triangles, triangle);
        }
    }

    // Sort the triangles by depth
    qsort(visible_triangles, array_length(visible_triangles), sizeof(triangle_t), compare_triangles_by_depth);
}

void render_scene(bool show_faces, bool show_edges, bool show_vertices) {
    // Draw visible triangles
    for (int i = 0; i < array_length(visible_triangles); i++) {
        triangle_t triangle = visible_triangles[i];
        face_t face = mesh.faces[i];
        draw_filled_triangle(
            triangle.points[0].x, triangle.points[0].y,
            triangle.points[1].x, triangle.points[1].y,
            triangle.points[2].x, triangle.points[2].y,
            face.color
        );
    }
}

