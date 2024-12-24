#include <math.h>
#include "vector.h"
#include <stdio.h>
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "transform.h"
#include "triangle.h"

// Vector 2D functions
float vec2_length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}
vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t result = {a.x + b.x, a.y + b.y};
    return result;
}
vec2_t vec2_sub(vec2_t a, vec2_t b) {
    vec2_t result = {a.x - b.x, a.y - b.y};
    return result;
}
vec2_t vec2_mul(vec2_t v, float factor) {
    vec2_t result = {v.x * factor, v.y * factor};
    return result;
}
vec2_t vec2_div(vec2_t v, float factor) {
    vec2_t result = {v.x / factor, v.y / factor};
    return result;
}
void vec2_normalize(vec2_t* v) {
    float length = vec2_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
    }
}

// Vector 3D functions
float vec3_length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}
vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}
vec3_t vec3_mul(vec3_t v, float factor) {
    vec3_t result = {v.x * factor, v.y * factor, v.z * factor};
    return result;
}
vec3_t vec3_div(vec3_t v, float factor) {
    vec3_t result = {v.x / factor, v.y / factor, v.z / factor};
    return result;
}
vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}
float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
void vec3_normalize(vec3_t* v) {
    float length = vec3_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
    }
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t result = {
        v.x,
        v.y * cos(angle) - v.z * sin(angle),
        v.y * sin(angle) + v.z * cos(angle)
    };
    return result;
}
vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t result = {
        v.x * cos(angle) + v.z * sin(angle),
        v.y,
        -v.x * sin(angle) + v.z * cos(angle)
    };
    return result;
}
vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t result = {
        v.x * cos(angle) - v.y * sin(angle),
        v.x * sin(angle) + v.y * cos(angle),
        v.z
    };
    return result;
}

// Vector conversion functions
vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t result = {v.x, v.y, v.z, 1.0f};
    return result;
}
vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t result = {v.x, v.y, v.z};
    return result;
}

vec3_t vec3_from_vec2(vec2_t v) {
    vec3_t result = {v.x, v.y, 0.0f};
    return result;
}

// Proyecta un vértice 3D al espacio 2D utilizando el FOV y la relación de aspecto
vec2_t project(vec3_t vertex, mat4_t world_matrix, mat4_t view_matrix, float aspect_ratio, float fov_factor) {
    // Transformar el vértice al espacio del mundo
    vec3_t transformed_vertex = mat4_mul_vec3(world_matrix, vertex);
    printf("Transformed Vertex: (%f, %f, %f)\n", transformed_vertex.x, transformed_vertex.y, transformed_vertex.z);

    // Transformar a espacio de cámara usando la matriz de vista
    vec3_t camera_vertex = mat4_mul_vec3(view_matrix, transformed_vertex);
    printf("Camera Vertex: (%f, %f, %f)\n", camera_vertex.x, camera_vertex.y, camera_vertex.z);

    // Aplicar la matriz de proyección
    mat4_t projection_matrix = mat4_make_perspective(fov_factor, aspect_ratio, 0.1f, 100.0f);
    vec4_t projected_vertex = mat4_mul_vec4(projection_matrix, vec4_from_vec3(camera_vertex));
    printf("Projected Vertex: (%f, %f, %f, %f)\n", projected_vertex.x, projected_vertex.y, projected_vertex.z, projected_vertex.w);

    // División perspectiva
    vec3_t ndc_vertex = perspective_divide(projected_vertex);
    printf("NDC Vertex: (%f, %f, %f)\n", ndc_vertex.x, ndc_vertex.y, ndc_vertex.z);

    // Corregir la inversión del eje y
    ndc_vertex.y *= -1;
    printf("Corrected NDC Vertex: (%f, %f, %f)\n", ndc_vertex.x, ndc_vertex.y, ndc_vertex.z);

    // Escalar y centrar
    vec2_t screen_vertex = scale_and_center(ndc_vertex, window_width, window_height);
    printf("Screen Vertex: (%f, %f)\n", screen_vertex.x, screen_vertex.y);

    return screen_vertex;
}

vec3_t vec3_scale(vec3_t v, float scale) {
    vec3_t result;
    result.x = v.x * scale;
    result.y = v.y * scale;
    result.z = v.z * scale;
    return result;
}

vec4_t apply_projection(vec3_t vertex, mat4_t projection_matrix) {
    vec4_t result = mat4_mul_vec4(projection_matrix, vec4_from_vec3(vertex));
    return result;
}

vec3_t perspective_divide(vec4_t vertex) {
    vec3_t result;
    if (vertex.w != 0) {
        result.x = vertex.x / vertex.w;
        result.y = vertex.y / vertex.w;
        result.z = vertex.z / vertex.w;
    } else {
        result.x = 0;
        result.y = 0;
        result.z = 0;
    }
    return result;
}

vec3_t correct_inversion(vec3_t vertex) {
    vertex.y *= -1;
    return vertex;
}

vec2_t scale_and_center(vec3_t vertex, int width, int height) {
    vec2_t result;
    result.x = (vertex.x + 1) * 0.5f * width;
    result.y = (vertex.y + 1) * 0.5f * height;
    return result;
}

vec4_t mat4_mul_vec4(mat4_t mat, vec4_t v) {
    vec4_t result;
    result.x = mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * v.w;
    result.y = mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * v.w;
    result.z = mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * v.w;
    result.w = mat.m[3][0] * v.x + mat.m[3][1] * v.y + mat.m[3][2] * v.z + mat.m[3][3] * v.w;
    return result;
}

vec3_t mat4_mul_vec3(mat4_t mat, vec3_t v) {
    vec3_t result;
    result.x = mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * 1.0f;
    result.y = mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * 1.0f;
    result.z = mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * 1.0f;
    return result;
}

vec2_t orthogonal_projection(vec3_t vertex, float aspect_ratio) {
    vec2_t result;
    result.x = vertex.x * aspect_ratio;
    result.y = vertex.y;
    return result;
}

vec3_t vec3_reflect(vec3_t l, vec3_t n) {
    float dotLN = vec3_dot(l, n);
    vec3_t result = {
        2 * dotLN * n.x - l.x,
        2 * dotLN * n.y - l.y,
        2 * dotLN * n.z - l.z
    };
    return result;
}

// Calcular el vector normal promedio en cada vértice
vec3_t calculate_vertex_normal(int vertex_index) {
    vec3_t normal_sum = {0, 0, 0};
    int adjacent_faces_count = 0;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];
        if (face.a == vertex_index || face.b == vertex_index || face.c == vertex_index) {
            vec3_t vertex_a = mesh.vertices[face.a - 1];
            vec3_t vertex_b = mesh.vertices[face.b - 1];
            vec3_t vertex_c = mesh.vertices[face.c - 1];
            vec3_t face_normal = calculate_normal(vertex_a, vertex_b, vertex_c);
            normal_sum = vec3_add(normal_sum, face_normal);
            adjacent_faces_count++;
        }
    }

    if (adjacent_faces_count > 0) {
        normal_sum = vec3_div(normal_sum, (float)adjacent_faces_count);
        vec3_normalize(&normal_sum);
    }

    return normal_sum;
}

