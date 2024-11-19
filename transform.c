//
// Created by robin on 31/10/2024.
//
#include "transform.h"

#include "matrix.h"
#include "vector.h"

void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment) {
    rotation->x += x_increment;
    rotation->y += y_increment;
    rotation->z += z_increment;
}

vec3_t transform_vertices_to_camera_space(mat4_t view_matrix, vec3_t vertex) {
    vec4_t transformed_vertex = mat4_mul_vec4(view_matrix, vec4_from_vec3(vertex));
    return vec3_from_vec4(transformed_vertex);
}
