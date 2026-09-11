#ifndef LIGHT_H
#define LIGHT_H

#include "vecmath.h"

#define LIGHT_PANEL_SAMPLES_X 2
#define LIGHT_PANEL_SAMPLES_Z 2
#define LIGHT_PANEL_SAMPLE_COUNT (LIGHT_PANEL_SAMPLES_X * LIGHT_PANEL_SAMPLES_Z)

Vec3 light_panel_sample(int index);

Vec3 light_panel_grid_point(int index, int columns, int rows);

// call with the view matrix loaded, GL stores light positions in eye space
void light_apply_base(int fill_enabled);
void light_apply_panel_grid(void);
void light_apply_panel_point(Vec3 position, float share);

#endif
