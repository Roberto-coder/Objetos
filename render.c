#include "render.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#include "triangle.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// Colors for elements
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Red for faces 0xFFFFFFFF;
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Green for edges
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Blue for vertices

// Define the light source
light_t light = {
    .direction = { 0, 0, -1 },
    .color = { 1.0, 1.0, 1.0 }, // Luz blanca
    .intensity = 1.0
};

vec2_t* projected_points = NULL;
face_t* visible_faces = NULL;

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

        vec3_normalize(&lightDir);
        vec3_normalize(&viewDir);

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

void render_scene(float aspect_ratio, float fov_factor, vec3_t camera_position) {
    face_t* faces_to_render = visible_faces;

    for (int i = 0; i < array_length(faces_to_render); i++) {
        face_t face = faces_to_render[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov_factor);

        if (apply_shading) {
            for (int y = min(projected_a.y, min(projected_b.y, projected_c.y)); y <= max(projected_a.y, max(projected_b.y, projected_c.y)); y++) {
                for (int x = min(projected_a.x, min(projected_b.x, projected_c.x)); x <= max(projected_a.x, max(projected_b.x, projected_c.x)); x++) {
                    float w1, w2, w3;
                    calculate_barycentric_weights((vec2_t){x, y}, projected_a, projected_b, projected_c, &w1, &w2, &w3);

                    if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                        uint32_t interpolated_color = barycentric_interpolation(vertex_a.intensity, vertex_b.intensity, vertex_c.intensity, w1, w2, w3);
                        draw_pixel(x, y, interpolated_color);
                    }
                }
            }
        } else {
            if (show_faces) {
                draw_filled_triangle(
                    projected_a.x, projected_a.y,
                    projected_b.x, projected_b.y,
                    projected_c.x, projected_c.y,
                    face.color
                );
            }

            if (show_edges) {
                draw_triangle(
                    projected_a.x, projected_a.y,
                    projected_b.x, projected_b.y,
                    projected_c.x, projected_c.y,
                    COLOR_ARISTAS
                );
            }

            if (show_vertices) {
                draw_pixel(projected_a.x, projected_a.y, COLOR_VERTICES);
                draw_pixel(projected_b.x, projected_b.y, COLOR_VERTICES);
                draw_pixel(projected_c.x, projected_c.y, COLOR_VERTICES);
            }
        }
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

uint32_t barycentric_interpolation(uint32_t i1, uint32_t i2, uint32_t i3, float w1, float w2, float w3) {
    uint32_t a = (i1 & 0xFF000000) * w1 + (i2 & 0xFF000000) * w2 + (i3 & 0xFF000000) * w3;
    uint32_t r = (i1 & 0x00FF0000) * w1 + (i2 & 0x00FF0000) * w2 + (i3 & 0x00FF0000) * w3;
    uint32_t g = (i1 & 0x0000FF00) * w1 + (i2 & 0x0000FF00) * w2 + (i3 & 0x0000FF00) * w3;
    uint32_t b = (i1 & 0x000000FF) * w1 + (i2 & 0x000000FF) * w2 + (i3 & 0x000000FF) * w3;
    return a | r | g | b;
}


// Calcular la intensidad en cada vértice usando el modelo de iluminación de Phong
void calculate_vertex_intensities(vec3_t camera_pos) {
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec3_t vertex = mesh.vertices[i];
        vec3_t normal = calculate_vertex_normal(i + 1); // Los índices de los vértices comienzan en 1
        vec3_t lightDir = vec3_sub(light.direction, vertex);
        vec3_t viewDir = vec3_sub(camera_pos, vertex);

        vec3_normalize(&lightDir);
        vec3_normalize(&viewDir);

        uint32_t ambient = 0xFF1A1A1A; // Ejemplo de color ambiente (negro)
        uint32_t diffuse = 0xFF808080; // Ejemplo de color difuso (gris obscuro)
        uint32_t specular = 0xFFFFFFFF; // Ejemplo de color especular (blanco)
        float shininess = 32.0;

        uint32_t intensity = phongIllumination(normal, lightDir, viewDir, ambient, diffuse, specular, shininess);
        mesh.vertices[i].intensity = intensity; // Asumimos que `vec3_t` tiene un campo `intensity`
    }
}

uint32_t phongIllumination(vec3_t normal, vec3_t lightDir, vec3_t viewDir, uint32_t ambient, uint32_t diffuse, uint32_t specular, float shininess) {
    // Normalizar los vectores
    vec3_normalize(&normal);
    vec3_normalize(&lightDir);
    vec3_normalize(&viewDir);

    // Debug: Imprimir los vectores normalizados
    //printf("Normal: (%f, %f, %f)\n", normal.x, normal.y, normal.z);
    //printf("LightDir: (%f, %f, %f)\n", lightDir.x, lightDir.y, lightDir.z);
    //printf("ViewDir: (%f, %f, %f)\n", viewDir.x, viewDir.y, viewDir.z);

    // Iluminación ambiente
    uint32_t ambientComponent = ambient;

    // Iluminación difusa
    float diff = fmax(vec3_dot(normal, lightDir), 0.0);
    uint32_t diffuseComponent = light_apply_intensity(diffuse, diff);

    // Debug: Imprimir la intensidad difusa
    //printf("Diffuse Intensity: %f\n", diff);

    // Iluminación especular
    vec3_t reflectDir = vec3_reflect(lightDir, normal);
    float spec = pow(fmax(vec3_dot(viewDir, reflectDir), 0.0), shininess);
    uint32_t specularComponent = light_apply_intensity(specular, spec);

    // Debug: Imprimir la intensidad especular
    //printf("Specular Intensity: %f\n", spec);

    // Sumar las componentes
    uint32_t result = ambientComponent + diffuseComponent + specularComponent;

    // Asegurarse de que el resultado no se desborde
    uint32_t a = (result & 0xFF000000);
    uint32_t r = fmin((result & 0x00FF0000), 0x00FF0000);
    uint32_t g = fmin((result & 0x0000FF00), 0x0000FF00);
    uint32_t b = fmin((result & 0x000000FF), 0x000000FF);

    // Debug: Imprimir el color resultante
    printf("Resulting Color: 0x%X\n", a | r | g | b);

    return a | r | g | b;
}

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