#include "material.h"

#include "gl_compat.h"

typedef struct {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat emission[4];
    GLfloat shininess;
} Material;

#define BLACK {0.0f, 0.0f, 0.0f, 1.0f}
#define MATTE(r, g, b) {{r, g, b, 1.0f}, {r, g, b, 1.0f}, BLACK, BLACK, 0.0f}

static const Material materials[] = {
    [MATERIAL_WHITE] = MATTE(0.725f, 0.710f, 0.680f),
    [MATERIAL_RED] = MATTE(0.630f, 0.065f, 0.050f),
    [MATERIAL_GREEN] = MATTE(0.140f, 0.450f, 0.091f),
    [MATERIAL_LIGHT] = {{0.78f, 0.78f, 0.78f, 1.0f}, {0.78f, 0.78f, 0.78f, 1.0f}, BLACK,
                        {1.0f, 0.97f, 0.90f, 1.0f}, 0.0f},
    [MATERIAL_GOLD] = {{0.25f, 0.20f, 0.07f, 1.0f}, {0.75f, 0.60f, 0.23f, 1.0f},
                       {0.63f, 0.56f, 0.37f, 1.0f}, BLACK, 51.0f},
    [MATERIAL_GLASS] = {{0.20f, 0.26f, 0.32f, 0.45f}, {0.55f, 0.70f, 0.85f, 0.45f},
                        {1.0f, 1.0f, 1.0f, 0.45f}, BLACK, 96.0f},
    [MATERIAL_WOOD] = {{0.22f, 0.14f, 0.08f, 1.0f}, {0.55f, 0.36f, 0.20f, 1.0f},
                       {0.20f, 0.15f, 0.10f, 1.0f}, BLACK, 12.0f},
    [MATERIAL_CANDLE] = {{0.35f, 0.34f, 0.31f, 1.0f}, {0.92f, 0.90f, 0.82f, 1.0f},
                         {0.30f, 0.30f, 0.30f, 1.0f}, BLACK, 20.0f},
    [MATERIAL_WICK] = MATTE(0.10f, 0.08f, 0.06f)
};

void material_apply(MaterialId material)
{
    const Material *m = &materials[material];

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m->ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m->diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m->specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m->emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m->shininess);
}
