//
// Created by robin on 31/10/2024.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"
#include "matrix.h"


// Declarar la función de rotación global
void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment);
vec2_t transform_vertex(vec3_t vertex, mat4_t world_matrix, vec3_t camera_position, float aspect_ratio, float fov_factor);
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up);

#endif //TRANSFORM_H
