#ifndef INC_3DRENDERER_MATRIX_H
#define INC_3DRENDERER_MATRIX_H

#include "vector.h"
#include "matrix.h"

typedef struct {
  float m[4][4];
} mat4_t;

// Variables para transformación
extern mat4_t world_matrix;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);



#endif //INC_3DRENDERER_MATRIX_H
