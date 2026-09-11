#ifndef DREIDEL_H
#define DREIDEL_H

#include "quad.h"

typedef struct {
    Quad side[4];
    Quad top;
    Vec3 tip[4][3];
} DreidelBody;

void dreidel_body(DreidelBody *out);

// must not run while a display list is being compiled, the texture upload would be recorded into it
void dreidel_init(void);
void dreidel_draw(int textured);

#endif
