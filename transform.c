//
// Created by robin on 31/10/2024.
//
#include "transform.h"
#include "vector.h"
#include "matrix.h"

void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment) {
    rotation->x += x_increment;
    rotation->y += y_increment;
    rotation->z += z_increment;
}

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
    vec2_t projected_vertex = {
        .x = (fov_factor * camera_vertex.x) / camera_vertex.z,
        .y = (fov_factor * camera_vertex.y) / (camera_vertex.z * aspect_ratio)
    };

    return projected_vertex;
}

vec3_t mat4_mul_vec3(mat4_t mat, vec3_t v) {
    vec3_t result;

    result.x = mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * 1.0f;
    result.y = mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * 1.0f;
    result.z = mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * 1.0f;

    return result;
}