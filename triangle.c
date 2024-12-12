#include "display.h"
#include "triangle.h"

#include "mesh.h"
#include "render.h"

// Dibuja un triángulo utilizando tres llamadas a draw_line
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t fill_color) {
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; x0 = x1; y0 = y1; x1 = tmp_x; y1 = tmp_y; }
    if (y1 > y2) { int tmp_x = x1, tmp_y = y1; x1 = x2; y1 = y2; x2 = tmp_x; y2 = tmp_y; }
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; x0 = x1; y0 = y1; x1 = tmp_x; y1 = tmp_y; }

    if (y1 == y2) {
        draw_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, fill_color);
    } else if (y0 == y1) {
        draw_flat_top_triangle(x0, y0, x1, y1, x2, y2, fill_color);
    } else {
        int x4 = x0 + ((float)(y1 - y0) / (float)(y2 - y0)) * (x2 - x0);
        int y4 = y1;

        draw_flat_bottom_triangle(x0, y0, x1, y1, x4, y4, fill_color);
        draw_flat_top_triangle(x1, y1, x4, y4, x2, y2, fill_color);
    }
}

void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    float cur_x1 = x0;
    float cur_x2 = x0;

    for (int y = y0; y <= y2; y++) {
        // Dibuja la línea completa entre cur_x1 y cur_x2
        draw_line((int)cur_x1, y, (int)cur_x2, y, color);

        cur_x1 += inv_slope_1;
        cur_x2 += inv_slope_2;
    }
}

void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    float cur_x1 = x2;
    float cur_x2 = x2;

    for (int y = y2; y >= y0; y--) {
        // Dibuja la línea completa entre cur_x1 y cur_x2
        draw_line((int)cur_x1, y, (int)cur_x2, y, color);

        cur_x1 -= inv_slope_1;
        cur_x2 -= inv_slope_2;
    }
}

// Función para calcular la profundidad promedio de una cara
/*float calculate_triangle_depth(face_t face) {
    vec3_t vertex_a = mesh.vertices[face.a - 1];
    vec3_t vertex_b = mesh.vertices[face.b - 1];
    vec3_t vertex_c = mesh.vertices[face.c - 1];
    return (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Promedio de profundidad
}*/

// Función de comparación para ordenar triángulos por profundidad (descendente)
int compare_triangles_by_depth(const void* a, const void* b) {
    triangle_t* tri_a = (triangle_t*)a;
    triangle_t* tri_b = (triangle_t*)b;
    return (tri_b->depth > tri_a->depth) - (tri_b->depth < tri_a->depth);
}


// Function to calculate the average depth of a triangle
float calculate_triangle_depth(face_t face) {
    vec3_t vertex_a = mesh.vertices[face.a - 1];
    vec3_t vertex_b = mesh.vertices[face.b - 1];
    vec3_t vertex_c = mesh.vertices[face.c - 1];
    return (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Average depth
}

vec3_t calculate_normal(vec3_t a, vec3_t b, vec3_t c) {
    vec3_t ab = vec3_sub(a, b);
    vec3_t ac = vec3_sub(a, c);
    vec3_t normal = vec3_cross(ab, ac);
    vec3_normalize(&normal); // Normaliza el vector utilizando el puntero
    return normal;
}

// Function to calculate the centroid of a triangle
vec3_t calculate_centroid(vec3_t vertex_a, vec3_t vertex_b, vec3_t vertex_c) {
    vec3_t centroid;
    centroid.x = (vertex_a.x + vertex_b.x + vertex_c.x) / 3.0f;
    centroid.y = (vertex_a.y + vertex_b.y + vertex_c.y) / 3.0f;
    centroid.z = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f;
    return centroid;
}


bool is_face_visible(vec3_t normal, vec3_t camera_pos, vec3_t vertex_a) {
    vec3_t view_vector = vec3_sub(camera_pos, vertex_a);
    float dot_product = vec3_dot(normal, view_vector);
    return dot_product < 0; // Visible si el producto escalar es negativo
}

