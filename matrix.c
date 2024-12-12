#include <math.h>
#include "vector.h"
#include "matrix.h"

// Crea una matriz identidad 4x4
mat4_t mat4_identity(void) {
    mat4_t result = {0};
    result.m[0][0] = 1;
    result.m[1][0] = 0;
    result.m[2][0] = 0;
    result.m[3][0] = 0;

    result.m[0][1] = 0;
    result.m[1][1] = 1;
    result.m[2][1] = 0;
    result.m[3][1] = 0;

    result.m[0][2] = 0;
    result.m[1][2] = 0;
    result.m[2][2] = 1;
    result.m[3][2] = 0;

    result.m[0][3] = 0;
    result.m[1][3] = 0;
    result.m[2][3] = 0;
    result.m[3][3] = 1;

    return result;
}

// Crea una matriz de escala 4x4
mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t result = mat4_identity();
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = sz;
    return result;
}

// Crea una matriz de traslación 4x4
mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t result = mat4_identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;
    return result;
}

// Calcular la matriz de proyección
mat4_t mat4_make_perspective(float fov, float aspect_ratio, float z_near, float z_far) {
    mat4_t result = {0};
    float f = 1.0f / tan(fov / 2.0f);
    float range = z_near - z_far;

    result.m[0][0] = f / aspect_ratio;
    result.m[1][1] = f;
    result.m[2][2] = (z_far + z_near) / range;
    result.m[2][3] = (2.0f * z_far * z_near) / range;
    result.m[3][2] = -1.0f;
    result.m[3][3] = 0.0f;

    return result;
}

// Crea una matriz de rotación en el eje X 4x4
mat4_t mat4_make_rotation_x(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

// Crea una matriz de rotación en el eje Y 4x4
mat4_t mat4_make_rotation_y(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;

    return result;
}

// Crea una matriz de rotación en el eje Z 4x4
mat4_t mat4_make_rotation_z(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;

    return result;
}

// Multiplica dos matrices 4x4
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t result = {0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] +
                             a.m[i][1] * b.m[1][j] +
                             a.m[i][2] * b.m[2][j] +
                             a.m[i][3] * b.m[3][j];
        }
    }

    return result;
}