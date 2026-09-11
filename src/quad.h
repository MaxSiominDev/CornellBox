#ifndef QUAD_H
#define QUAD_H

#include "vecmath.h"

typedef struct {
    Vec3 corner[4];
} Quad;

Quad quad_make(Vec3 c0, Vec3 c1, Vec3 c2, Vec3 c3);

Vec3 quad_point(const Quad *quad, float u, float v);
Vec3 quad_normal(const Quad *quad, float u, float v);
float quad_area(const Quad *quad);

#endif
