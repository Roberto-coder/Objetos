#include "display.h"
#include "triangle.h"
#include "render.h"

triangle_t triangles[MAX_TRIANGLES];  // Definición de la variable global
int triangle_count = 0;  // Definición del contador de triángulos, inicializado a 0

// Dibuja un triángulo utilizando tres llamadas a draw_line
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t fill_color) {
    // Ordena los puntos para que y0 <= y1 <= y2
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; x0 = x1; y0 = y1; x1 = tmp_x; y1 = tmp_y; }
    if (y1 > y2) { int tmp_x = x1, tmp_y = y1; x1 = x2; y1 = y2; x2 = tmp_x; y2 = tmp_y; }
    if (y0 > y1) { int tmp_x = x0, tmp_y = y0; x0 = x1; y0 = y1; x1 = tmp_x; y1 = tmp_y; }

    if (y1 == y2) {
        // Triángulo flat-bottom
        draw_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, fill_color);
    } else if (y0 == y1) {
        // Triángulo flat-top
        draw_flat_top_triangle(x0, y0, x1, y1, x2, y2, fill_color);
    } else {
        // Triángulo general: se divide en dos, creando un punto de división
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
