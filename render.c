#include "render.h"
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#include <stdio.h>
#include <math.h>

// Definir la fuente de luz
light_t light = {
    .direction = { 0, 0, 1 }
};

// Declarar variables globales
vec2_t* projected_points = NULL;
triangle_t* all_triangles = NULL;
triangle_t* visible_triangles = NULL;

const uint32_t COLOR_CARAS = 0xFFFFFFFF;

// Declarar variables externas
extern bool apply_shading; // Declarar la variable externa para controlar el sombreado

// Función para aplicar la intensidad de la luz al color original
uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {
    if (percentage_factor < 0) percentage_factor = 0;
    if (percentage_factor > 1) percentage_factor = 1;

    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = ((original_color & 0x00FF0000) >> 16) * percentage_factor;
    uint32_t g = ((original_color & 0x0000FF00) >> 8) * percentage_factor;
    uint32_t b = (original_color & 0x000000FF) * percentage_factor;

    uint32_t new_color = a | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    return new_color;
}

// Función para calcular las caras visibles
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling) {
    float aspect_ratio = (float)window_width / (float)window_height;

    if (all_triangles != NULL) {
        array_free(all_triangles);
    }
    all_triangles = NULL;

    if (visible_triangles != NULL) {
        array_free(visible_triangles);
    }
    visible_triangles = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        // Calcular el vector normal de la cara usando el producto cruz de AB y AC, y luego normalizarlo
        vec3_t vector_ab = vec3_sub(vertex_b, vertex_a);
        vec3_t vector_ac = vec3_sub(vertex_c, vertex_a);
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);
        printf("Normal for face %d: (%f, %f, %f)\n", i, normal.x, normal.y, normal.z);

        // Calcular el factor de intensidad de la luz
        float light_intensity_factor = fabs(vec3_dot(normal, light.direction));
        printf("Light intensity factor for face %d: %f\n", i, light_intensity_factor);

        // Aplicar la intensidad de la luz al color de la cara
        uint32_t original_color = COLOR_CARAS;
        uint32_t new_color = light_apply_intensity(original_color, light_intensity_factor);
        printf("Original color: 0x%X, New color: 0x%X\n", original_color, new_color);

        // Proyectar los vértices
        vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov);
        vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov);
        vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov);

        face.depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Calcular la profundidad

        triangle_t triangle = {
            .points = {projected_a, projected_b, projected_c},
            .depth = face.depth
        };

        array_push(all_triangles, triangle);

        // Verificar si la cara es visible
        if (!back_face_culling || is_face_visible(normal, camera_pos, vertex_a)) {
            array_push(visible_triangles, triangle);
        }
    }

    // Ordenar todos los triángulos por profundidad
    qsort(all_triangles, array_length(all_triangles), sizeof(triangle_t), compare_triangles_by_depth);

    // Ordenar los triángulos visibles por profundidad
    qsort(visible_triangles, array_length(visible_triangles), sizeof(triangle_t), compare_triangles_by_depth);
}

// Función para renderizar la escena
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool back_face_culling) {
    triangle_t* triangles_to_render = back_face_culling ? visible_triangles : all_triangles;

    // Dibujar triángulos
    for (int i = 0; i < array_length(triangles_to_render); i++) {
        triangle_t triangle = triangles_to_render[i];
        face_t face = mesh.faces[i];
        uint32_t original_color = COLOR_CARAS;
        uint32_t new_color = original_color;

        if (apply_shading) { // Aplicar sombreado si apply_shading es verdadero
            vec3_t vertex_a = mesh.vertices[face.a - 1];
            vec3_t vertex_b = mesh.vertices[face.b - 1];
            vec3_t vertex_c = mesh.vertices[face.c - 1];

            vec3_t vector_ab = vec3_sub(vertex_b, vertex_a);
            vec3_t vector_ac = vec3_sub(vertex_c, vertex_a);
            vec3_t normal = vec3_cross(vector_ab, vector_ac);
            vec3_normalize(&normal);

            float light_intensity_factor = vec3_dot(normal, light.direction);
            new_color = light_apply_intensity(original_color, light_intensity_factor);
        }

        draw_filled_triangle(
            triangle.points[0].x, triangle.points[0].y,
            triangle.points[1].x, triangle.points[1].y,
            triangle.points[2].x, triangle.points[2].y,
            new_color
        );
    }
}