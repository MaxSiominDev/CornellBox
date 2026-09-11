#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl_compat.h"

GLuint texture_from_luminance(const unsigned char *pixels, int width, int height);

GLuint texture_brushed_metal(int size);

#endif
