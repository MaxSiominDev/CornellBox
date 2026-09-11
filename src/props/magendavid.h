#ifndef MAGENDAVID_H
#define MAGENDAVID_H

#include "quad.h"

void magendavid_init(void);
void magendavid_draw_pedestal(void);
void magendavid_draw_star(void);

void magendavid_bar_quads(Vec3 a, Vec3 b, float thickness, Vec3 up, Quad out[6]);

#endif
