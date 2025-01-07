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

// Función de comparación para ordenar caras por profundidad (descendente)
int compare_faces_by_depth(const void* a, const void* b) {
    face_t* face_a = (face_t*)a;
    face_t* face_b = (face_t*)b;
    return (face_b->depth > face_a->depth) - (face_b->depth < face_a->depth);
}

// Función de comparación para ordenar caras por profundidad (ascendente)
int compare_faces_by_depth_asc(const void* a, const void* b) {
    face_t* face_a = (face_t*)a;
    face_t* face_b = (face_t*)b;
    return (face_a->depth > face_b->depth) - (face_a->depth < face_b->depth);
}

vec3_t calculate_normal(vec3_t a, vec3_t b, vec3_t c) {
    vec3_t ab = vec3_sub(a, b);
    vec3_t ac = vec3_sub(a, c);
    vec3_t normal = vec3_cross(ab, ac);
    vec3_normalize(&normal); // Normaliza el vector utilizando el puntero
    return normal;
}


bool is_face_visible(vec3_t normal, vec3_t camera_pos, vec3_t vertex) {
    vec3_t view_vector = vec3_sub(camera_pos, vertex);
    return vec3_dot(normal, view_vector) < 0;
}

void draw_filled_triangle_gouraud(int x0, int y0, float i0, int x1, int y1, float i1, int x2, int y2, float i2, uint32_t color) {
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; float tmp_i = i0; x0 = x1; y0 = y1; i0 = i1; x1 = tmp_x; y1 = tmp_y; i1 = tmp_i; }
    if (y1 > y2) { int tmp_x = x1, tmp_y = y1; float tmp_i = i1; x1 = x2; y1 = y2; i1 = i2; x2 = tmp_x; y2 = tmp_y; i2 = tmp_i; }
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; float tmp_i = i0; x0 = x1; y0 = y1; i0 = i1; x1 = tmp_x; y1 = tmp_y; i1 = tmp_i; }

    if (y1 == y2) {
        draw_flat_bottom_triangle_gouraud(x0, y0, i0, x1, y1, i1, x2, y2, i2, color);
    } else if (y0 == y1) {
        draw_flat_top_triangle_gouraud(x0, y0, i0, x1, y1, i1, x2, y2, i2, color);
    } else {
        int x4 = x0 + ((float)(y1 - y0) / (float)(y2 - y0)) * (x2 - x0);
        float i4 = i0 + ((float)(y1 - y0) / (float)(y2 - y0)) * (i2 - i0);
        int y4 = y1;

        draw_flat_bottom_triangle_gouraud(x0, y0, i0, x1, y1, i1, x4, y4, i4, color);
        draw_flat_top_triangle_gouraud(x1, y1, i1, x4, y4, i4, x2, y2, i2, color);
    }
}

void draw_flat_bottom_triangle_gouraud(int x0, int y0, float i0, int x1, int y1, float i1, int x2, int y2, float i2, uint32_t color) {
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    float inv_intensity_1 = (i1 - i0) / (y1 - y0);
    float inv_intensity_2 = (i2 - i0) / (y2 - y0);

    float cur_x1 = x0;
    float cur_x2 = x0;
    float cur_i1 = i0;
    float cur_i2 = i0;

    for (int y = y0; y <= y1; y++) {
        draw_line_gouraud((int)cur_x1, y, cur_i1, (int)cur_x2, y, cur_i2, color);
        cur_x1 += inv_slope_1;
        cur_x2 += inv_slope_2;
        cur_i1 += inv_intensity_1;
        cur_i2 += inv_intensity_2;
    }
}

void draw_flat_top_triangle_gouraud(int x0, int y0, float i0, int x1, int y1, float i1, int x2, int y2, float i2, uint32_t color) {
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);
    float inv_intensity_1 = (i2 - i0) / (y2 - y0);
    float inv_intensity_2 = (i2 - i1) / (y2 - y1);

    float cur_x1 = x2;
    float cur_x2 = x2;
    float cur_i1 = i2;
    float cur_i2 = i2;

    for (int y = y2; y >= y0; y--) {
        draw_line_gouraud((int)cur_x1, y, cur_i1, (int)cur_x2, y, cur_i2, color);
        cur_x1 -= inv_slope_1;
        cur_x2 -= inv_slope_2;
        cur_i1 -= inv_intensity_1;
        cur_i2 -= inv_intensity_2;
    }
}

void draw_line_gouraud(int x0, int y0, float i0, int x1, int y1, float i1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        float t = (dx > dy) ? (float)(x0 - x1) / dx : (float)(y0 - y1) / dy;
        uint32_t interpolated_color = linear_interpolation(color, 0xFF000000, i0 + t * (i1 - i0));
        draw_pixel(x0, y0, interpolated_color);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}