#include "render.h"
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#include <stdio.h>
#include <math.h>

// Define the light source
light_t light = {
    .direction = { 0, 0, 1 },
    .color = { 1.0, 1.0, 1.0 }, // Luz blanca
    .intensity = 1.0
};

// Declare global variables
vec2_t* projected_points = NULL;
face_t* visible_faces = NULL;

const uint32_t COLOR_CARAS = 0xFFFFFFFF;

mat4_t view_matrix; // Define view_matrix
mat4_t world_matrix; // Define world_matrix

extern bool show_faces;
extern bool show_edges;
extern bool show_vertices;
extern bool apply_shading;
extern bool back_face_culling;

// Function to apply light intensity to the original color
uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {
    if (percentage_factor < 0) percentage_factor = 0;
    if (percentage_factor > 1) percentage_factor = 1;

    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = ((original_color & 0x00FF0000) >> 16) * percentage_factor;
    uint32_t g = ((original_color & 0x0000FF00) >> 8) * percentage_factor;
    uint32_t b = (original_color & 0x000000FF) * percentage_factor;

    uint32_t new_color = a | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    return new_color;
}

uint32_t phongIllumination(vec3_t normal, vec3_t lightDir, vec3_t viewDir, uint32_t ambient, uint32_t diffuse, uint32_t specular, float shininess) {
    // Normalizar los vectores
    vec3_normalize(&normal);
    vec3_normalize(&lightDir);
    vec3_normalize(&viewDir);

    // Iluminación ambiente
    uint32_t ambientComponent = ambient;

    // Iluminación difusa
    float diff = fmax(vec3_dot(normal, lightDir), 0.0);
    uint32_t diffuseComponent = light_apply_intensity(diffuse, diff);

    // Iluminación especular
    vec3_t reflectDir = vec3_reflect(lightDir, normal);
    float spec = pow(fmax(vec3_dot(viewDir, reflectDir), 0.0), shininess);
    uint32_t specularComponent = light_apply_intensity(specular, spec);

    // Sumar las componentes
    uint32_t result = ambientComponent + diffuseComponent + specularComponent;

    return result;
}

// Calcular la intensidad en cada vértice usando el modelo de iluminación de Phong
void calculate_vertex_intensities(vec3_t camera_pos) {
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec3_t vertex = mesh.vertices[i];
        vec3_t normal = calculate_vertex_normal(i + 1); // Los índices de los vértices comienzan en 1
        vec3_t lightDir = vec3_sub(light.direction, vertex);
        vec3_t viewDir = vec3_sub(camera_pos, vertex);

        uint32_t ambient = 0xFF1A1A1A; // Ejemplo de color ambiente
        uint32_t diffuse = 0xFF808080; // Ejemplo de color difuso
        uint32_t specular = 0xFFFFFFFF; // Ejemplo de color especular
        float shininess = 32.0;

        uint32_t intensity = phongIllumination(normal, lightDir, viewDir, ambient, diffuse, specular, shininess);
        mesh.vertices[i].intensity = intensity; // Asumimos que `vec3_t` tiene un campo `intensity`
    }
}

// Interpolación lineal
uint32_t linear_interpolation(uint32_t start, uint32_t end, float t) {
    uint32_t a = (start & 0xFF000000) + t * ((end & 0xFF000000) - (start & 0xFF000000));
    uint32_t r = (start & 0x00FF0000) + t * ((end & 0x00FF0000) - (start & 0x00FF0000));
    uint32_t g = (start & 0x0000FF00) + t * ((end & 0x0000FF00) - (start & 0x0000FF00));
    uint32_t b = (start & 0x000000FF) + t * ((end & 0x000000FF) - (start & 0x000000FF));
    return a | r | g | b;
}

// Interpolación para coordenadas baricéntricas (comentario)
/*
uint32_t barycentric_interpolation(uint32_t i1, uint32_t i2, uint32_t i3, float w1, float w2, float w3) {
    uint32_t a = (i1 & 0xFF000000) * w1 + (i2 & 0xFF000000) * w2 + (i3 & 0xFF000000) * w3;
    uint32_t r = (i1 & 0x00FF0000) * w1 + (i2 & 0x00FF0000) * w2 + (i3 & 0x00FF0000) * w3;
    uint32_t g = (i1 & 0x0000FF00) * w1 + (i2 & 0x0000FF00) * w2 + (i3 & 0x0000FF00) * w3;
    uint32_t b = (i1 & 0x000000FF) * w1 + (i2 & 0x000000FF) * w2 + (i3 & 0x000000FF) * w3;
    return a | r | g | b;
}
*/

// Function to calculate visible faces
void calculate_visible_faces(vec3_t camera_pos, float fov, bool back_face_culling) {

    if (visible_faces != NULL) {
        array_free(visible_faces);
    }
    visible_faces = NULL;

    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t face = mesh.faces[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        // Transform vertices using the world matrix
        vertex_a = mat4_mul_vec3(world_matrix, vertex_a);
        vertex_b = mat4_mul_vec3(world_matrix, vertex_b);
        vertex_c = mat4_mul_vec3(world_matrix, vertex_c);

        // Transform vertices using the view matrix
        vertex_a = mat4_mul_vec3(view_matrix, vertex_a);
        vertex_b = mat4_mul_vec3(view_matrix, vertex_b);
        vertex_c = mat4_mul_vec3(view_matrix, vertex_c);

        // Calculate the normal of the face
        vec3_t normal = calculate_normal(vertex_a, vertex_b, vertex_c);

        // Calculate the light direction and view direction
        vec3_t lightDir = vec3_sub(light.direction, vertex_a);
        vec3_t viewDir = vec3_sub(camera_pos, vertex_a);

        // Calculate the Phong illumination
        uint32_t ambient = 0xFF1A1A1A; // Example ambient color
        uint32_t diffuse = 0xFF808080; // Example diffuse color
        uint32_t specular = 0xFFFFFFFF; // Example specular color
        float shininess = 32.0;
        uint32_t phongColor = phongIllumination(normal, lightDir, viewDir, ambient, diffuse, specular, shininess);

        // Apply light intensity to the face color
        face.color = phongColor; // Update face color with shading
        face.depth = (vertex_a.z + vertex_b.z + vertex_c.z) / 3.0f; // Calculate depth

        // Check if the face is visible
        if (!back_face_culling || is_face_visible(normal, camera_pos, vertex_a)) {
            array_push(visible_faces, face);
        }
    }

    // Sort visible faces by depth
    qsort(visible_faces, array_length(visible_faces), sizeof(face_t), compare_faces_by_depth);
}

// Function to render the scene
void render_scene(bool show_faces, bool show_edges, bool show_vertices, bool back_face_culling, bool apply_shading, float aspect_ratio, float fov_factor, vec3_t camera_position) {
    face_t* faces_to_render = visible_faces;

    // Draw faces
    if (show_faces) {
        for (int i = 0; i < array_length(faces_to_render); i++) {
            face_t face = faces_to_render[i];

            vec3_t vertex_a = mesh.vertices[face.a - 1];
            vec3_t vertex_b = mesh.vertices[face.b - 1];
            vec3_t vertex_c = mesh.vertices[face.c - 1];

            vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov_factor);
            vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov_factor);
            vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov_factor);

            uint32_t face_color = face.color;
            if (apply_shading) {
                // Interpolate the intensity values of the vertices
                float intensity_a = vertex_a.intensity;
                float intensity_b = vertex_b.intensity;
                float intensity_c = vertex_c.intensity;

                // Calculate the interpolated intensity for the face
                float t = 0.5f; // Example interpolation factor
                float interpolated_intensity = (intensity_a + intensity_b + intensity_c) / 3.0f;

                // Apply the interpolated intensity to the face color
                face_color = linear_interpolation(face.color, 0xFF000000, interpolated_intensity);
            }

            // Check if the face is visible based on back-face culling
            if (!back_face_culling || is_face_visible(calculate_normal(vertex_a, vertex_b, vertex_c), camera_position, vertex_a)) {
                draw_filled_triangle(
                    projected_a.x, projected_a.y,
                    projected_b.x, projected_b.y,
                    projected_c.x, projected_c.y,
                    face_color
                );
            }
        }
    }

    // Draw edges
    if (show_edges) {
        for (int i = 0; i < array_length(faces_to_render); i++) {
            face_t face = faces_to_render[i];

            vec3_t vertex_a = mesh.vertices[face.a - 1];
            vec3_t vertex_b = mesh.vertices[face.b - 1];
            vec3_t vertex_c = mesh.vertices[face.c - 1];

            vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov_factor);
            vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov_factor);
            vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov_factor);

            draw_triangle(
                projected_a.x, projected_a.y,
                projected_b.x, projected_b.y,
                projected_c.x, projected_c.y,
                0xFF000000 // Black color for edges
            );
        }
    }

    // Draw vertices
    if (show_vertices) {
        for (int i = 0; i < array_length(mesh.vertices); i++) {
            vec3_t vertex = mesh.vertices[i];
            vec2_t projected = project(vertex, world_matrix, view_matrix, aspect_ratio, fov_factor);
            draw_pixel(projected.x, projected.y, 0xFFFF0000); // Red color for vertices
        }
    }
}



