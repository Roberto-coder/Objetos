#include <stdio.h>
#include <string.h>
#include "array.h"
#include "mesh.h"
#include <stdlib.h>
#include <stdint.h>

mesh_t mesh = {
        .vertices = NULL,
        .faces = NULL
};

// Array of predefined colors
uint32_t predefined_colors[6] = {
    0xFFFFFFFF,
    0xFFFFFFFF, 
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
};

// Generate a color from the predefined array
uint32_t generate_random_color() {
    static int color_index = 0;
    uint32_t color = predefined_colors[color_index];
    color_index = (color_index + 1) % 6; // Cycle through the colors
    return color;
}
/*
// Generar un color aleatorio
uint32_t generate_random_color() {
    uint8_t r = rand() % 256;
    uint8_t g = rand() % 256;
    uint8_t b = rand() % 256;
    return (r << 24) | (g << 16) | (b << 8) | 0xFF; // Formato RGBA
}*/

vec3_t cube_vertices[N_CUBE_VERTICES] = {
        { .x = -1, .y = -1, .z = -1 }, // 1
        { .x = -1, .y =  1, .z = -1 }, // 2
        { .x =  1, .y =  1, .z = -1 }, // 3
        { .x =  1, .y = -1, .z = -1 }, // 4
        { .x =  1, .y =  1, .z =  1 }, // 5
        { .x =  1, .y = -1, .z =  1 }, // 6
        { .x = -1, .y =  1, .z =  1 }, // 7
        { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
        // front
        { .a = 1, .b = 2, .c = 3, .color = 0xFFFF0000 },
        { .a = 1, .b = 3, .c = 4, .color = 0xFFFF0000 },
        // right
        { .a = 4, .b = 3, .c = 5, .color = 0xFF00FF00 },
        { .a = 4, .b = 5, .c = 6, .color = 0xFF00FF00 },
        // back
        { .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF },
        { .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF },
        // left
        { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00 },
        { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00 },
        // top
        { .a = 2, .b = 7, .c = 5, .color = 0xFFFF0000 },
        { .a = 2, .b = 5, .c = 3, .color = 0xFFFF0000 },
        // bottom
        { .a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF },
        { .a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF }
};

void load_cube_mesh_data(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
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
            printf("Vertex loaded: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
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
                .c = vertex_indices[2]
                //.color = generate_random_color() // Asignar un color aleatorio
            };

            array_push(mesh.faces, face);
            printf("Face loaded: (%d, %d, %d) with color: 0x%X\n", face.a, face.b, face.c, face.color);
        }
    }

    fclose(file);
}
