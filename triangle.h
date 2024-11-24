#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

typedef struct {
    int a;
    int b;
    int c;
    uint32_t color;
} face_t;

typedef struct {
    vec2_t points[3];
    float depth;
} triangle_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t fill_color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

float calculate_triangle_depth(face_t face);
int compare_triangles_by_depth(const void* a, const void* b);
bool is_face_visible(vec3_t normal, vec3_t camera_pos, vec3_t vertex);


vec3_t calculate_normal(vec3_t a, vec3_t b, vec3_t c);


#endif //INC_3DRENDERER_TRIANGLE_H
