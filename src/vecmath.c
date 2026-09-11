#include "vecmath.h"

#include <math.h>

Vec3 v3(float x, float y, float z)
{
    Vec3 result;

    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

Vec3 v3_add(Vec3 a, Vec3 b)
{
    return v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 v3_sub(Vec3 a, Vec3 b)
{
    return v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 v3_scale(Vec3 v, float k)
{
    return v3(v.x * k, v.y * k, v.z * k);
}

Vec3 v3_lerp(Vec3 a, Vec3 b, float t)
{
    return v3_add(a, v3_scale(v3_sub(b, a), t));
}

Vec3 v3_cross(Vec3 a, Vec3 b)
{
    return v3(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

float v3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float v3_length(Vec3 v)
{
    return sqrtf(v3_dot(v, v));
}

Vec3 v3_normalize(Vec3 v)
{
    const float length = v3_length(v);

    return length > 0.0f ? v3_scale(v, 1.0f / length) : v;
}
