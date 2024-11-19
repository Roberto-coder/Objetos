//
// Created by robin on 17/11/2024.
//
#include "camera.h"
#include <math.h>

// Crear una cámara con posición, objetivo y vector arriba


mat4_t mat4_make_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    // Dirección hacia el objetivo
    vec3_t f = vec3_sub(center, eye);
    vec3_normalize(&f);  // Normalizar el vector f

    // Eje derecho (producto cruzado de up y f)
    vec3_t r = vec3_cross(up, f);
    vec3_normalize(&r);  // Normalizar el vector r

    // Eje "arriba" real (producto cruzado de f y r)
    vec3_t u = vec3_cross(f, r);
    vec3_normalize(&u);  // Normalizar el vector u

    // Crear la matriz de vista 4x4
    mat4_t view_matrix = {0};

    // Llenamos la matriz de vista
    view_matrix.m[0][0] = r.x;
    view_matrix.m[1][0] = r.y;
    view_matrix.m[2][0] = r.z;
    view_matrix.m[3][0] = -vec3_dot(r, eye);  // Producto punto de r y eye

    view_matrix.m[0][1] = u.x;
    view_matrix.m[1][1] = u.y;
    view_matrix.m[2][1] = u.z;
    view_matrix.m[3][1] = -vec3_dot(u, eye);  // Producto punto de u y eye

    view_matrix.m[0][2] = -f.x;
    view_matrix.m[1][2] = -f.y;
    view_matrix.m[2][2] = -f.z;
    view_matrix.m[3][2] = vec3_dot(f, eye);  // Producto punto de f y eye

    view_matrix.m[0][3] = 0.0f;
    view_matrix.m[1][3] = 0.0f;
    view_matrix.m[2][3] = 0.0f;
    view_matrix.m[3][3] = 1.0f;

    return view_matrix;
}

