#include "render.h"
#include <stdio.h>
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#define MAX_DEPTH 1000.0f

// Colors for elements
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Red for faces
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Green for edges
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Blue for vertices

vec2_t* projected_points = NULL;
triangle_t* visible_triangles = NULL;

// Function to calculate visible faces and sort them by depth
void calculate_visible_faces(vec3_t camera_pos, float fov) {

    if (visible_triangles != NULL) {
        array_free(visible_triangles);
    }
    visible_triangles = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec3_t vertex_a = mat4_mul_vec3(world_matrix, mesh.vertices[face.a - 1]);
        vec3_t vertex_b = mat4_mul_vec3(world_matrix, mesh.vertices[face.b - 1]);
        vec3_t vertex_c = mat4_mul_vec3(world_matrix, mesh.vertices[face.c - 1]);

        // Calculate the normal of the face
        vec3_t normal = calculate_normal(vertex_a, vertex_b, vertex_c);

        // Determine if the face is visible from the camera position
        if (is_face_visible(normal, camera_pos, vertex_a)) {
            // Calculate the depth of the triangle
            float depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f;

            vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov);
            vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov);
            vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov);

            triangle_t triangle = {
                .points = {projected_a, projected_b, projected_c},
                .depth = depth,
                .color = face.color, // Use the color from the face
                .normal = normal // Assign the normal
            };

            array_push(visible_triangles, triangle);
        }
    }

    // Sort the visible triangles by depth
    qsort(visible_triangles, array_length(visible_triangles), sizeof(triangle_t), compare_triangles_by_depth);

    // Print the sorted depths for debugging
    /*printf("Sorted triangle depths:\n");
    for (int i = 0; i < array_length(visible_triangles); i++) {
        printf("Triangle %d depth: %f\n", i, visible_triangles[i].depth);
    }*/
}

// Function to render the scene
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool backface_culling_enabled) {
    for (int i = 0; i < array_length(visible_triangles); i++) {
        triangle_t triangle = visible_triangles[i];

        if (show_faces) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color // Use the color of the face
            );
        }

        if (show_edges) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_ARISTAS
            );
        }

        if (show_vertices) {
            draw_pixel(triangle.points[0].x, triangle.points[0].y, COLOR_VERTICES);
            draw_pixel(triangle.points[1].x, triangle.points[1].y, COLOR_VERTICES);
            draw_pixel(triangle.points[2].x, triangle.points[2].y, COLOR_VERTICES);
        }


    }
}