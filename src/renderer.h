#ifndef RENDERER_H
#define RENDERER_H

#include "app.h"

void renderer_init(void);

void renderer_draw_frame(const AppState *app, int width, int height);

#endif
