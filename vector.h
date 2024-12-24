#ifndef INC_3DRENDERER_VECTOR_H
#define INC_3DRENDERER_VECTOR_H

#include <stdint.h>

#include "vector.h"
#include "matrix.h"

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
    uint32_t intensity;
} vec3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;


// Vector 2D functions
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
vec2_t project(vec3_t vertex, mat4_t world_matrix, mat4_t view_matrix, float aspect_ratio, float fov_factor);
//vec2_t project(vec3_t vertex, mat4_t world_matrix, mat4_t view_matrix, float aspect_ratio, float fov_factor);
void vec2_normalize(vec2_t* v);

// Vector 3D functions
float vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t v, float factor);
vec3_t vec3_div(vec3_t v, float factor);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);
void vec3_normalize(vec3_t* v);

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

// Vector conversion functions
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);

vec3_t vec3_from_vec2(vec2_t v);

vec3_t vec3_scale(vec3_t v, float scale);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

vec4_t apply_projection(vec3_t vertex, mat4_t projection_matrix);
vec3_t perspective_divide(vec4_t vertex) ;

vec3_t correct_inversion(vec3_t vertex);
vec2_t scale_and_center(vec3_t vertex, int width, int height);

vec3_t mat4_mul_vec3(mat4_t mat, vec3_t v);
vec4_t mat4_mul_vec4(mat4_t mat, vec4_t v); // Add this line

vec2_t orthogonal_projection(vec3_t vertex, float aspect_ratio);

vec3_t vec3_reflect(vec3_t l, vec3_t n);
vec3_t calculate_vertex_normal(int vertex_index);
//vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

#endif //INC_3DRENDERER_VECTOR_H
