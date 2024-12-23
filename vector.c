
#include <math.h>
#include "vector.h"

#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "transform.h"

// Vector 2D functions
float vec2_length(vec2_t v){
    return sqrt(v.x * v.x + v.y * v.y);
}
vec2_t vec2_add(vec2_t a, vec2_t b){
    vec2_t result = {a.x + b.x, a.y + b.y};
    return result;
}
vec2_t vec2_sub(vec2_t a, vec2_t b){
    vec2_t result = {a.x - b.x, a.y - b.y};
    return result;
}
vec2_t vec2_mul(vec2_t v, float factor){
    vec2_t result = {v.x * factor, v.y * factor};
    return result;
}
vec2_t vec2_div(vec2_t v, float factor){
    vec2_t result = {v.x / factor, v.y / factor};
    return result;
}
void vec2_normalize(vec2_t* v){// Normaliza un vector 2D (lo convierte en un vector unitario)
    float length = vec2_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
    }
}

// Vector 3D functions
float vec3_length(vec3_t v){
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
vec3_t vec3_add(vec3_t a, vec3_t b){
  vec3_t result = {a.x + b.x,a.y + b.y,a.z + b.z};
  return result;
}
vec3_t vec3_sub(vec3_t a, vec3_t b){
    vec3_t result = {a.x - b.x,a.y - b.y,a.z - b.z};
    return result;
}
vec3_t vec3_mul(vec3_t v, float factor){
    vec3_t result = {v.x * factor, v.y * factor, v.z * factor};
    return result;
}
vec3_t vec3_div(vec3_t v, float factor){
    vec3_t result = {v.x / factor, v.y / factor, v.z / factor};
    return result;
}
vec3_t vec3_cross(vec3_t a, vec3_t b){//Producto cruz(determinante)
  //  | ax ay az |  =  |ay az|i + |ax az|j + |ax ay|k
  //  | bx by bz |     |by bz|    |bx bz|    |bx by|
    vec3_t result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}
float vec3_dot(vec3_t a, vec3_t b){//Producto punto
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
void vec3_normalize(vec3_t* v){
    float length = vec3_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
    }
}

vec3_t vec3_rotate_x(vec3_t v, float angle){
    vec3_t result = {
        v.x,
        v.y * cos(angle) - v.z * sin(angle),
        v.y * sin(angle) + v.z * cos(angle)
    };
    return result;
}
vec3_t vec3_rotate_y(vec3_t v, float angle){
    vec3_t result = {
        v.x * cos(angle) + v.z * sin(angle),
        v.y,
        -v.x * sin(angle) + v.z * cos(angle)
    };
    return result;
}
vec3_t vec3_rotate_z(vec3_t v, float angle){
    vec3_t result = {
        v.x * cos(angle) - v.y * sin(angle),
        v.x * sin(angle) + v.y * cos(angle),
        v.z
    };
    return result;
}

// Vector conversion functions
vec4_t vec4_from_vec3(vec3_t v){
    vec4_t result = {v.x, v.y, v.z, 1.0f}; // W generalmente es 1 en gráficos 3D
    return result;
}
vec3_t vec3_from_vec4(vec4_t v){
    vec3_t result = {v.x, v.y, v.z}; // Se ignora el componente W
    return result;
}

// Proyecta un vértice 3D al espacio 2D utilizando el FOV y la relación de aspecto
/*vec2_t project(vec3_t vertex, float fov_factor, float aspect_ratio) {
    vec2_t projected_vertex;

    if (vertex.z != 0) {
        // Proyección en perspectiva
        projected_vertex.x = (fov_factor * vertex.x / vertex.z) * aspect_ratio;
        projected_vertex.y = (fov_factor * vertex.y / vertex.z);
    } else {
        // Evitar divisiones por 0
        projected_vertex.x = 0;
        projected_vertex.y = 0;
    }

    return projected_vertex;
}*/

vec2_t project(vec3_t vertex, mat4_t world_matrix, mat4_t view_matrix, float aspect_ratio, float fov_factor) {
    // Transformar el vértice al espacio del mundo
    vec3_t transformed_vertex = mat4_mul_vec3(world_matrix, vertex);

    // Transformar a espacio de cámara usando la matriz de vista
    vec3_t camera_vertex = mat4_mul_vec3(view_matrix, transformed_vertex);

    vec2_t projected_vertex;

    if (camera_vertex.z != 0) {
        // Proyección en perspectiva
        projected_vertex.x = (fov_factor * camera_vertex.x / camera_vertex.z) * aspect_ratio;
        projected_vertex.y = (fov_factor * camera_vertex.y / camera_vertex.z);
    } else {
        // Evitar divisiones por 0
        projected_vertex.x = 0;
        projected_vertex.y = 0;
    }

    return projected_vertex;
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


