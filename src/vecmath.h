#ifndef VECMATH_H
#define VECMATH_H

#define VEC_PI 3.14159265358979f

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

Vec3 v3(float x, float y, float z);
Vec3 v3_add(Vec3 a, Vec3 b);
Vec3 v3_sub(Vec3 a, Vec3 b);
Vec3 v3_scale(Vec3 v, float k);
Vec3 v3_lerp(Vec3 a, Vec3 b, float t);
Vec3 v3_cross(Vec3 a, Vec3 b);
float v3_dot(Vec3 a, Vec3 b);
float v3_length(Vec3 v);
Vec3 v3_normalize(Vec3 v);

#endif
