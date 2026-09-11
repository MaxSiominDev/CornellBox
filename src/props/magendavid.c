#include "magendavid.h"

#include "gl_compat.h"
#include "material.h"
#include "mesh.h"

#include <math.h>

#define SLICES 24
#define STAR_RADIUS 55.0f
#define BAR_THICKNESS 6.0f
#define PEDESTAL_RADIUS 18.0f
#define PEDESTAL_HEIGHT 6.0f
#define POST_RADIUS 5.0f
#define FRAME_OFFSET 3.5f

static GLUquadric *quadric;

void magendavid_init(void)
{
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
}

void magendavid_bar_quads(Vec3 a, Vec3 b, float thickness, Vec3 up, Quad out[6])
{
    const float half = 0.5f * thickness;
    const Vec3 along = v3_normalize(v3_sub(b, a));
    const Vec3 side = v3_normalize(v3_cross(along, up));
    const Vec3 u = v3_scale(v3_normalize(v3_cross(side, along)), half);
    const Vec3 s = v3_scale(side, half);
    const Vec3 c[8] = {
        v3_sub(v3_sub(a, s), u), v3_sub(v3_add(a, s), u), v3_add(v3_add(a, s), u), v3_add(v3_sub(a, s), u),
        v3_sub(v3_sub(b, s), u), v3_sub(v3_add(b, s), u), v3_add(v3_add(b, s), u), v3_add(v3_sub(b, s), u)
    };

    out[0] = quad_make(c[0], c[1], c[2], c[3]);
    out[1] = quad_make(c[7], c[6], c[5], c[4]);
    out[2] = quad_make(c[0], c[4], c[5], c[1]);
    out[3] = quad_make(c[1], c[5], c[6], c[2]);
    out[4] = quad_make(c[2], c[6], c[7], c[3]);
    out[5] = quad_make(c[3], c[7], c[4], c[0]);
}

static void draw_bar(Vec3 a, Vec3 b)
{
    Quad faces[6];
    int i;

    magendavid_bar_quads(a, b, BAR_THICKNESS, v3(0.0f, 0.0f, 1.0f), faces);
    for (i = 0; i < 6; i++) {
        mesh_draw_quad(&faces[i], 1);
    }
}

static void draw_triangle(float center_y, float z, float phase)
{
    Vec3 corner[3];
    int i;

    for (i = 0; i < 3; i++) {
        const float angle = phase + 2.0f * VEC_PI * (float)i / 3.0f;

        corner[i] = v3(STAR_RADIUS * cosf(angle), center_y + STAR_RADIUS * sinf(angle), z);
    }
    for (i = 0; i < 3; i++) {
        draw_bar(corner[i], corner[(i + 1) % 3]);
    }
}

#define STAR_CENTER_Y (PEDESTAL_HEIGHT + STAR_RADIUS + 2.0f)

void magendavid_draw_pedestal(void)
{
    material_apply(MATERIAL_GOLD);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluDisk(quadric, 0.0, PEDESTAL_RADIUS, SLICES, 1);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluCylinder(quadric, PEDESTAL_RADIUS, PEDESTAL_RADIUS * 0.8, PEDESTAL_HEIGHT, SLICES, 1);
    glTranslatef(0.0f, 0.0f, PEDESTAL_HEIGHT);
    gluDisk(quadric, 0.0, PEDESTAL_RADIUS * 0.8, SLICES, 1);
    gluCylinder(quadric, POST_RADIUS, POST_RADIUS,
                STAR_CENTER_Y - STAR_RADIUS + BAR_THICKNESS - PEDESTAL_HEIGHT, SLICES, 1);
    glPopMatrix();
}

void magendavid_draw_star(void)
{
    material_apply(MATERIAL_GLASS);
    draw_triangle(STAR_CENTER_Y, FRAME_OFFSET, 0.5f * VEC_PI);
    draw_triangle(STAR_CENTER_Y, -FRAME_OFFSET, -0.5f * VEC_PI);
}
