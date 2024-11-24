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

void render_scene(vec3_t camera_pos, float fov, bool show_faces, bool show_edges, bool show_vertices) {
    float aspect_ratio = (float)window_width / (float)window_height;

    if (visible_triangles != NULL) {
        array_free(visible_triangles);
    }
    visible_triangles = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        // Usar la función transformada que retorna vec2_t
        vec2_t vertex_a = transform_vertex(mesh.vertices[face.a - 1], world_matrix, camera_pos, aspect_ratio, fov);
        vec2_t vertex_b = transform_vertex(mesh.vertices[face.b - 1], world_matrix, camera_pos, aspect_ratio, fov);
        vec2_t vertex_c = transform_vertex(mesh.vertices[face.c - 1], world_matrix, camera_pos, aspect_ratio, fov);

        // Determinar si la cara es visible: usa el cálculo de normal directamente en vec2 si es necesario.
        // Opcionalmente, ajusta lógica de normal si aún necesitas un vec3_t intermedio.
        // OMITIDO AQUÍ: La lógica para verificar visibilidad necesitaría un ajuste adicional.

        float face_depth = (mesh.vertices[face.a - 1].z +
                            mesh.vertices[face.b - 1].z +
                            mesh.vertices[face.c - 1].z) / 3.0f;

        triangle_t triangle = {
            .points = {vertex_a, vertex_b, vertex_c},
            .depth = face_depth
        };

        array_push(visible_triangles, triangle);
    }

    // Ordenar los triángulos por profundidad
    qsort(visible_triangles, array_length(visible_triangles), sizeof(triangle_t), compare_triangles_by_depth);

    // Dibujar triángulos visibles
    for (int i = 0; i < array_length(visible_triangles); i++) {
        triangle_t triangle = visible_triangles[i];
        draw_filled_triangle(
            triangle.points[0].x, triangle.points[0].y,
            triangle.points[1].x, triangle.points[1].y,
            triangle.points[2].x, triangle.points[2].y,
            COLOR_CARAS
        );
    }
}
