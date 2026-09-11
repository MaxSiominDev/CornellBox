#include "quad.h"

Quad quad_make(Vec3 c0, Vec3 c1, Vec3 c2, Vec3 c3)
{
    Quad quad;

    quad.corner[0] = c0;
    quad.corner[1] = c1;
    quad.corner[2] = c2;
    quad.corner[3] = c3;

    return quad;
}

Vec3 quad_point(const Quad *quad, float u, float v)
{
    const Vec3 near_edge = v3_lerp(quad->corner[0], quad->corner[1], u);
    const Vec3 far_edge = v3_lerp(quad->corner[3], quad->corner[2], u);

    return v3_lerp(near_edge, far_edge, v);
}

Vec3 quad_normal(const Quad *quad, float u, float v)
{
    const Vec3 tangent_u = v3_lerp(v3_sub(quad->corner[1], quad->corner[0]),
                                   v3_sub(quad->corner[2], quad->corner[3]), v);
    const Vec3 tangent_v = v3_lerp(v3_sub(quad->corner[3], quad->corner[0]),
                                   v3_sub(quad->corner[2], quad->corner[1]), u);

    return v3_normalize(v3_cross(tangent_u, tangent_v));
}

float quad_area(const Quad *quad)
{
    const Vec3 first = v3_cross(v3_sub(quad->corner[1], quad->corner[0]),
                                v3_sub(quad->corner[2], quad->corner[0]));
    const Vec3 second = v3_cross(v3_sub(quad->corner[2], quad->corner[0]),
                                 v3_sub(quad->corner[3], quad->corner[0]));

    return 0.5f * (v3_length(first) + v3_length(second));
}
