#include <stdio.h>
#include <string.h>
#include "array.h"
#include "mesh.h"
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

mesh_t mesh = {
        .vertices = NULL,
        .faces = NULL
};

// Array of predefined colors
uint32_t predefined_colors[6] = {
    0xFFF5B041, // Naranja
    0xFFCD5C5C, // IndianRed
    0xFFF08080, // LightCoral
    0xFFFA8072, // Salmon
    0xFFE9967A, // DarkSalmon
    0xFFFFA07A  // LightSalmon
};

// Generate a color from the predefined array
/*uint32_t generate_random_color() {
    static int color_index = 0;
    uint32_t color = predefined_colors[color_index];
    color_index = (color_index + 1) % 6; // Cycle through the colors
    return color;
}*/

// Generar un color aleatorio
uint32_t generate_random_color() {
    uint8_t r = rand() % 256;
    uint8_t g = rand() % 256;
    uint8_t b = rand() % 256;
    return (r << 24) | (g << 16) | (b << 8) | 0xFF; // Formato RGBA
}


void load_obj_file_data(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error loading OBJ file: %s\n", filename);
        return;
    }

    char line[1024];
    srand(time(NULL)); // Inicializa la semilla para generar colores aleatorios

    while (fgets(line, sizeof(line), file)) {
        // Verificar si la línea define un vértice
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
            //printf("Vertex loaded: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
        }
        // Verificar si la línea define una cara
        else if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];  // Opcional, se ignora en este caso
            int normal_indices[3];   // Opcional, se ignora en este caso

            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );

            face_t face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2],
                .color = generate_random_color() // Asignar un color aleatorio
            };

            array_push(mesh.faces, face);
            //printf("Face loaded: (%d, %d, %d) with color: 0x%X\n", face.a, face.b, face.c, face.color);
        }
    }

    fclose(file);
}
