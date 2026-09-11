#ifndef CAMERA_H
#define CAMERA_H

#include "vecmath.h"

void camera_reset(void);
void camera_orbit(float yaw_degrees, float pitch_degrees);
void camera_zoom(float factor);
Vec3 camera_eye(void);

void camera_apply_projection(int width, int height);
void camera_apply_view(void);

#endif
