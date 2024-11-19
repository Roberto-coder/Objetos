#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"
#include "render.h"

// Declaración de la variable global de los vértices
extern vec3_t vertices[MAX_VERTICES]; // Declarar la variable globalmente

// Declaración de la variable que contiene el número de vértices
extern int vertex_count;

typedef struct {
    int a;
    int b;
    int c;
    color_t color;
} face_t;

// Estructura para un triángulo en 3D
typedef struct {
    vec3_t vertices[3]; // Coordenadas de los vértices
    float depth;        // Profundidad promedio del triángulo
} triangle_t;

extern triangle_t triangles[MAX_TRIANGLES];  // Declarar la variable globalmente
extern int triangle_count;  // Contador de triángulos, inicializado a 0


void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t fill_color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif //INC_3DRENDERER_TRIANGLE_H
