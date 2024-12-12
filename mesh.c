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
    0xFFB71C1C, // Dark Red
    0xFF1B5E20, // Dark Green
    0xFF0D47A1, // Dark Blue
    0xFFF57F17, // Mustard Yellow
    0xFF4A148C, // Dark Purple
    0xFFBF360C  // Burnt Orange
};

// Generate a color from the predefined array
uint32_t generate_random_color() {
    static int color_index = 0;
    uint32_t color = predefined_colors[color_index];
    color_index = (color_index + 1) % 6; // Cycle through the colors
    return color;
}

void load_obj_file_data(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error loading OBJ file: %s\n", filename);
        return;
    }

    char line[1024];
    srand(time(NULL)); // Initialize the seed for random color generation

    while (fgets(line, sizeof(line), file)) {
        // Check if the line defines a vertex
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
            printf("Vertex loaded: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
        }
        // Check if the line defines a face
        else if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];  // Optional, ignored in this case
            int normal_indices[3];   // Optional, ignored in this case

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
                .color = generate_random_color() // Assign a random color
            };

            array_push(mesh.faces, face);
            printf("Face loaded: (%d, %d, %d) with color: 0x%X\n", face.a, face.b, face.c, face.color);
        }
    }

    fclose(file);
}

// Function to get the number of faces in the mesh
int get_num_faces() {
    return array_length(mesh.faces);
}