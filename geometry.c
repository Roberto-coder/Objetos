#include "matrix.h"
#include "triangle.h"
#include "geometry.h"
#include "vector.h"



// Calcula la profundidad promedio de un triángulo
float calculate_triangle_depth(vec3_t a, vec3_t b, vec3_t c) {
    return (a.z + b.z + c.z) / 3.0f;
}

// Función de comparación para ordenar triángulos por profundidad
int compare_triangles(const void* a, const void* b) {
    const triangle_t* tri_a = (const triangle_t*)a;
    const triangle_t* tri_b = (const triangle_t*)b;

    float depth_a = calculate_triangle_depth(tri_a->vertices[0], tri_a->vertices[1], tri_a->vertices[2]);
    float depth_b = calculate_triangle_depth(tri_b->vertices[0], tri_b->vertices[1], tri_b->vertices[2]);

    if (depth_a < depth_b) return 1;  // Mayor profundidad primero
    if (depth_a > depth_b) return -1;
    return 0;
}

// Ordena los triángulos por profundidad
void sort_triangles_by_depth(triangle_t* triangles, int triangle_count) {
    qsort(triangles, triangle_count, sizeof(triangle_t), compare_triangles);
}

// Determina si un triángulo es visible desde la cámara
bool is_visible(vec3_t a, vec3_t b, vec3_t c, vec3_t camera_position) {
    vec3_t u = vec3_sub(b, a);
    vec3_t v = vec3_sub(c, a);
    vec3_t normal = vec3_cross(u, v);
    vec3_normalize(&normal); // Pasar por referencia para normalizar

    vec3_t camera_ray = vec3_sub(camera_position, a);
    vec3_normalize(&camera_ray); // Pasar por referencia para normalizar

    return vec3_dot(normal, camera_ray) < 0;
}
