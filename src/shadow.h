#ifndef SHADOW_H
#define SHADOW_H

#include "vecmath.h"

int shadow_sample_count(void);
Vec3 shadow_sample_position(int sample);

int shadow_init(const char **reason);

void shadow_render_depth(int sample, void (*draw)(void *), void *context, int width, int height);

void shadow_bind(int sample);
void shadow_unbind(void);

#endif
