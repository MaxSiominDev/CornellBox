#include "hebrew_glyphs.h"

void hebrew_glyph_unpack(int glyph, unsigned char ink, unsigned char paper, unsigned char *out)
{
    const unsigned char *bits = hebrew_glyph_bits[glyph];
    int i;

    for (i = 0; i < HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE; i++) {
        const int set = bits[i / 8] & (0x80 >> (i % 8));

        out[i] = set ? ink : paper;
    }
}
