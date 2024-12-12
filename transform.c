//
// Created by robin on 31/10/2024.
//
#include "transform.h"

#include <SDL_stdinc.h>

#include "display.h"
#include "vector.h"
#include "matrix.h"

void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment) {
    rotation->x += x_increment;
    rotation->y += y_increment;
    rotation->z += z_increment;
}

// Transforma un vértice 3D aplicando la matriz de transformación y lo proyecta en 2D
#include "transform.h"
#include "vector.h"
#include "matrix.h"

// Transforma un vértice 3D aplicando la matriz de transformación y lo proyecta en 2D
vec2_t transform_vertex(
    vec3_t vertex,
    mat4_t world_matrix,
    vec3_t camera_position,
    float aspect_ratio,
    float fov_factor
) {
    // Transformar el vértice al espacio del mundo
    vec3_t transformed_vertex = mat4_mul_vec3(world_matrix, vertex);

    // Transformar a espacio de cámara (restar la posición de la cámara)
    vec3_t camera_vertex = vec3_sub(transformed_vertex, camera_position);

    // Proyección perspectiva
    vec4_t projected_vertex = mat4_mul_vec4(mat4_make_perspective(M_PI / 3, aspect_ratio, 0.1f, 100.0f), (vec4_t){camera_vertex.x, camera_vertex.y, camera_vertex.z, 1.0f});

    // Perspective divide
    if (projected_vertex.w != 0) {
        projected_vertex.x /= projected_vertex.w;
        projected_vertex.y /= projected_vertex.w;
        projected_vertex.z /= projected_vertex.w;
    }

    // Corregir la inversión del eje y
    projected_vertex.y *= -1;
    // Corregir la inversión del eje z
    projected_vertex.x *= -1;

    // Escalar y centrar
    projected_vertex.x = (projected_vertex.x + 1) * (window_width / 2.0f);
    projected_vertex.y = (projected_vertex.y + 1) * (window_height / 2.0f);

    return (vec2_t){projected_vertex.x, projected_vertex.y};
}


// Crea una matriz de vista para una cámara en la posición eye, mirando hacia center, con un vector up
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    vec3_t f = vec3_sub(center, eye);
    vec3_normalize(&f);
    vec3_t s = vec3_cross(f, up);
    vec3_normalize(&s);
    vec3_t u = vec3_cross(s, f);

    mat4_t result = mat4_identity();
    result.m[0][0] = s.x;
    result.m[0][1] = s.y;
    result.m[0][2] = s.z;
    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;
    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;
    result.m[0][3] = -vec3_dot(s, eye);
    result.m[1][3] = -vec3_dot(u, eye);
    result.m[2][3] = vec3_dot(f, eye);

    return result;
}

