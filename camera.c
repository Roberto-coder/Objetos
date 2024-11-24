//
// Created by robin on 17/11/2024.
//
#include "camera.h"
#include <math.h>

camera_t camera_create(vec3_t position, vec3_t target, vec3_t up) {
    camera_t camera;
    camera.position = position;
    camera.target = target;
    camera.up = up;
    return camera;
}


mat4_t camera_get_view_matrix(camera_t camera) {
    return mat4_make_look_at(camera.position, camera.target, camera.up);
}

mat4_t mat4_make_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    // Declarar los vectores a normalizar
    vec3_t forward = vec3_sub(center, eye);
    vec3_t right = vec3_cross(up, forward);

    // Normalizar los vectores
    vec3_normalize(&forward);
    vec3_normalize(&right);

    // Calcular el vector "up" real
    vec3_t true_up = vec3_cross(forward, right);

    // Construir la matriz de rotación
    mat4_t rotation = {{
        {right.x, true_up.x, forward.x, 0},
        {right.y, true_up.y, forward.y, 0},
        {right.z, true_up.z, forward.z, 0},
        {0, 0, 0, 1}
    }};

    // Construir la matriz de traslación
    mat4_t translation = mat4_make_translation(-eye.x, -eye.y, -eye.z);

    // Combinar rotación y traslación
    return mat4_mul_mat4(rotation, translation);
}

