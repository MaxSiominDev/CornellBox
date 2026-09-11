#ifndef MATERIAL_H
#define MATERIAL_H

typedef enum {
    MATERIAL_WHITE,
    MATERIAL_RED,
    MATERIAL_GREEN,
    MATERIAL_LIGHT,
    MATERIAL_GOLD,
    MATERIAL_GLASS,
    MATERIAL_WOOD,
    MATERIAL_CANDLE,
    MATERIAL_WICK
} MaterialId;

void material_apply(MaterialId material);

#endif
