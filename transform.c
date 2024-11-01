//
// Created by robin on 31/10/2024.
//
#include "transform.h"
#include "vector.h"

void rotate_object(vec3_t* rotation, float x_increment, float y_increment, float z_increment) {
    rotation->x += x_increment;
    rotation->y += y_increment;
    rotation->z += z_increment;
}
