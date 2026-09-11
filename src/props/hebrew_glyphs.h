#ifndef HEBREW_GLYPHS_H
#define HEBREW_GLYPHS_H

#define HEBREW_GLYPH_SIZE 96
#define HEBREW_GLYPH_COUNT 4
#define HEBREW_GLYPH_BYTES (HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE / 8)

extern const unsigned char hebrew_glyph_bits[HEBREW_GLYPH_COUNT][HEBREW_GLYPH_BYTES];

void hebrew_glyph_unpack(int glyph, unsigned char ink, unsigned char paper, unsigned char *out);

#endif
