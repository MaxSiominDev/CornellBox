#ifndef MESH_H
#define MESH_H

#include "quad.h"

void mesh_draw_quad(const Quad *quad, int steps);
void mesh_draw_quad_textured(const Quad *quad, int steps);

#endif
