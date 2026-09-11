#include "texture.h"

#include <stdlib.h>

#define STREAK_LENGTH 24
#define STREAK_DEPTH 0.22f

GLuint texture_from_luminance(const unsigned char *pixels, int width, int height)
{
    GLuint id = 0;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

static unsigned int next_random(unsigned int *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state >> 8;
}

GLuint texture_brushed_metal(int size)
{
    float *noise = malloc(sizeof *noise * (size_t)size * (size_t)size);
    unsigned char *pixels = malloc((size_t)size * (size_t)size);
    unsigned int state = 12345u;
    GLuint id;
    int x;
    int y;

    if (noise == NULL || pixels == NULL) {
        free(noise);
        free(pixels);
        return 0;
    }

    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            noise[y * size + x] = (float)(next_random(&state) & 0xffff) / 65535.0f;
        }
    }

    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            float sum = 0.0f;
            int k;

            for (k = -STREAK_LENGTH / 2; k < STREAK_LENGTH / 2; k++) {
                sum += noise[((y + k + size) % size) * size + x];
            }
            pixels[y * size + x] =
                (unsigned char)(255.0f * (1.0f - STREAK_DEPTH * (sum / (float)STREAK_LENGTH)));
        }
    }

    id = texture_from_luminance(pixels, size, size);
    free(noise);
    free(pixels);

    return id;
}
