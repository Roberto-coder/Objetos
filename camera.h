//
// Created by robin on 17/11/2024.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

// Estructura para representar una cámara
typedef struct {
    vec3_t position;  // Posición de la cámara en el espacio
    vec3_t target;    // Hacia dónde apunta la cámara
    vec3_t up;        // Vector hacia arriba
} camera_t;

// Funciones de la cámara
camera_t camera_create(vec3_t position, vec3_t target, vec3_t up);
mat4_t camera_get_view_matrix(camera_t camera);
mat4_t mat4_make_look_at(vec3_t eye, vec3_t center, vec3_t up);

#endif //CAMERA_H
