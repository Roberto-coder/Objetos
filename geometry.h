#ifndef INC_3DRENDERER_GEOMETRY_H
#define INC_3DRENDERER_GEOMETRY_H

#include "matrix.h"



// Calcula la profundidad promedio de un triángulo
float calculate_triangle_depth(vec3_t a, vec3_t b, vec3_t c);

// Ordena los triángulos por profundidad
void sort_triangles_by_depth(triangle_t* triangles, int triangle_count);

// Determina si un triángulo es visible
bool is_visible(vec3_t a, vec3_t b, vec3_t c, vec3_t camera_position);

#endif // INC_3DRENDERER_GEOMETRY_H
