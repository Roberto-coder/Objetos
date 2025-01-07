#include "render.h"
#include <math.h>
#include "mesh.h"
#include "vector.h"
#include "array.h"
#include "transform.h"
#include "triangle.h"

// Colors for elements
const uint32_t COLOR_CARAS = 0xFFFF0000;    // Red for faces 0xFFFFFFFF;
const uint32_t COLOR_ARISTAS = 0xFF00FF00;  // Green for edges
const uint32_t COLOR_VERTICES = 0xFF0000FF; // Blue for vertices

// Define the light source
light_t light = {
    .direction = { 0, 0, -10 },
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

        // Calculate the Phong illumination
        uint32_t ambient = 0xFF1A1A1A; // Example ambient color (negro no tan obscuro)
        uint32_t diffuse = 0xFF808080; // Example diffuse color (gris)
        uint32_t specular = 0xFFFFFFFF; // Example specular color (blanco)
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
    calculate_vertex_intensities(camera_position);
    face_t* faces_to_render = visible_faces;

    for (int i = 0; i < array_length(faces_to_render); i++) {
        face_t face = faces_to_render[i];

        vec3_t vertex_a = mesh.vertices[face.a - 1];
        vec3_t vertex_b = mesh.vertices[face.b - 1];
        vec3_t vertex_c = mesh.vertices[face.c - 1];

        vec2_t projected_a = project(vertex_a, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_b = project(vertex_b, world_matrix, view_matrix, aspect_ratio, fov_factor);
        vec2_t projected_c = project(vertex_c, world_matrix, view_matrix, aspect_ratio, fov_factor);

        if (!back_face_culling || is_face_visible(calculate_normal(vertex_a, vertex_b, vertex_c), camera_position, vertex_a)) {
            if (show_faces) {
                if (apply_shading) {
                    draw_filled_triangle_gouraud(
                        projected_a.x, projected_a.y, vertex_a.intensity,
                        projected_b.x, projected_b.y, vertex_b.intensity,
                        projected_c.x, projected_c.y, vertex_c.intensity,
                        face.color
                    );
                } /*else {
                    draw_filled_triangle(
                        projected_a.x, projected_a.y,
                        projected_b.x, projected_b.y,
                        projected_c.x, projected_c.y,
                        face.color
                    );
                }*/
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

// Calcular la intensidad en cada vértice usando el modelo de iluminación de Phong
void calculate_vertex_intensities(vec3_t camera_pos) {
    for (int i = 0; i < array_length(mesh.vertices); i++) {
        vec3_t vertex = mesh.vertices[i];
        vec3_t normal = calculate_vertex_normal(i);

        vec3_t lightDir = vec3_sub(light.direction, vertex);
        vec3_t viewDir = vec3_sub(camera_pos, vertex);

        uint32_t ambient = 0xFF1A1A1A;
        uint32_t diffuse = 0xFF808080;
        uint32_t specular = 0xFFFFFFFF;
        float shininess = 32.0;

        uint32_t intensity = phongIllumination(normal, lightDir, viewDir, ambient, diffuse, specular, shininess);
        mesh.vertices[i].intensity = intensity;
    }
}

uint32_t phongIllumination(vec3_t normal, vec3_t lightDir, vec3_t viewDir, uint32_t ambient, uint32_t diffuse, uint32_t specular, float shininess) {
    // Normalizar los vectores
    vec3_normalize(&normal);
    vec3_normalize(&lightDir);
    vec3_normalize(&viewDir);
    // Calcular el término difuso
    float diff = vec3_dot(normal, lightDir);
    if (diff < 0.0f) {
        diff = 0.0f; // Asegurarse de que no haya contribución negativa
    }

    // Calcular el término especular
    vec3_t reflectDir = vec3_reflect(lightDir, normal);
    float spec = vec3_dot(reflectDir, viewDir);
    if (spec < 0.0f) {
        spec = 0.0f; // Asegurarse de que no haya contribución negativa
    }
    spec = powf(spec, shininess);

    // Separar los componentes RGB de los colores de entrada
    uint8_t ambientR = (ambient >> 16) & 0xFF;
    uint8_t ambientG = (ambient >> 8) & 0xFF;
    uint8_t ambientB = ambient & 0xFF;

    uint8_t diffuseR = (diffuse >> 16) & 0xFF;
    uint8_t diffuseG = (diffuse >> 8) & 0xFF;
    uint8_t diffuseB = diffuse & 0xFF;

    uint8_t specularR = (specular >> 16) & 0xFF;
    uint8_t specularG = (specular >> 8) & 0xFF;
    uint8_t specularB = specular & 0xFF;

    // Combinar los componentes utilizando los términos de iluminación
    uint8_t finalR = (uint8_t)fminf(ambientR + diff * diffuseR + spec * specularR, 255.0f);
    uint8_t finalG = (uint8_t)fminf(ambientG + diff * diffuseG + spec * specularG, 255.0f);
    uint8_t finalB = (uint8_t)fminf(ambientB + diff * diffuseB + spec * specularB, 255.0f);

    // Recombinar los componentes RGB en un único valor de 32 bits
    return (finalR << 16) | (finalG << 8) | finalB;
}


