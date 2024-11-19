//
// Created by robin on 31/10/2024.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H


#include "matrix.h"
#include "vector.h"

// Declarar la función de rotación global
void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment);
vec3_t transform_vertices_to_camera_space(mat4_t view_matrix, vec3_t vertex);
#endif //TRANSFORM_H
