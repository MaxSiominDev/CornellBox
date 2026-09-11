#include "menorah.h"

#include "gl_compat.h"
#include "material.h"
#include "texture.h"
#include "vecmath.h"

#include <math.h>

#define SLICES 16
#define JOINT_SLICES 10
#define ARC_SEGMENTS 8

#define BASE_RADIUS 30.0f
#define BASE_HEIGHT 6.0f
#define STEM_RADIUS 5.0f
#define ARM_RADIUS 4.0f
#define KNOB_RADIUS 8.0f
#define ARC_CENTER_HEIGHT 90.0f
#define ARM_SPACING 20.0f
#define CUP_HEIGHT 120.0f
#define CUP_RIM_RADIUS 10.0f
#define CUP_DEPTH 12.0f
#define CANDLE_RADIUS 4.0f
#define CANDLE_HEIGHT 28.0f
#define WICK_HEIGHT 4.0f
#define METAL_TEXTURE_SIZE 256

static GLUquadric *quadric;
static GLuint metal_texture;

void menorah_init(void)
{
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
    metal_texture = texture_brushed_metal(METAL_TEXTURE_SIZE);
}

static void begin_metal(int textured)
{
    material_apply(MATERIAL_GOLD);
    if (!textured) {
        return;
    }
    glBindTexture(GL_TEXTURE_2D, metal_texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
}

static void end_metal(void)
{
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void draw_tube(Vec3 a, Vec3 b, float radius)
{
    const Vec3 delta = v3_sub(b, a);
    const float length = v3_length(delta);
    const Vec3 direction = v3_scale(delta, 1.0f / length);
    const Vec3 axis = v3_cross(v3(0.0f, 0.0f, 1.0f), direction);
    const float angle = acosf(direction.z) * 180.0f / VEC_PI;

    glPushMatrix();
    glTranslatef(a.x, a.y, a.z);
    if (v3_length(axis) > 1e-6f) {
        glRotatef(angle, axis.x, axis.y, axis.z);
    } else if (direction.z < 0.0f) {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }
    gluCylinder(quadric, radius, radius, length, SLICES, 1);
    glPopMatrix();
}

static void draw_post(float x, float y0, float y1, float radius)
{
    glPushMatrix();
    glTranslatef(x, y0, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluDisk(quadric, 0.0, radius, SLICES, 1);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, y1 - y0, SLICES, 1);
    glTranslatef(0.0f, 0.0f, y1 - y0);
    gluDisk(quadric, 0.0, radius, SLICES, 1);
    glPopMatrix();
}

static void draw_sphere(float x, float y, float radius)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    gluSphere(quadric, radius, JOINT_SLICES, JOINT_SLICES / 2);
    glPopMatrix();
}

static void draw_arm(float side, float arc_radius)
{
    const Vec3 center = v3(0.0f, ARC_CENTER_HEIGHT, 0.0f);
    Vec3 previous = v3(0.0f, ARC_CENTER_HEIGHT - arc_radius, 0.0f);
    int i;

    for (i = 1; i <= ARC_SEGMENTS; i++) {
        const float angle = -0.5f * VEC_PI + 0.5f * VEC_PI * (float)i / ARC_SEGMENTS;
        const Vec3 point = v3_add(center, v3(side * arc_radius * cosf(angle),
                                             arc_radius * sinf(angle), 0.0f));

        draw_tube(previous, point, ARM_RADIUS);
        draw_sphere(point.x, point.y, ARM_RADIUS);
        previous = point;
    }

    draw_post(side * arc_radius, ARC_CENTER_HEIGHT, CUP_HEIGHT, ARM_RADIUS);
}

static void draw_cup_and_candle(float x, int textured)
{
    glPushMatrix();
    glTranslatef(x, CUP_HEIGHT, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    begin_metal(textured);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluDisk(quadric, 0.0, ARM_RADIUS, SLICES, 1);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluCylinder(quadric, ARM_RADIUS, CUP_RIM_RADIUS, CUP_DEPTH, SLICES, 1);
    glTranslatef(0.0f, 0.0f, CUP_DEPTH);
    gluDisk(quadric, CANDLE_RADIUS, CUP_RIM_RADIUS, SLICES, 1);
    end_metal();

    material_apply(MATERIAL_CANDLE);
    gluCylinder(quadric, CANDLE_RADIUS, CANDLE_RADIUS, CANDLE_HEIGHT, SLICES, 1);
    glTranslatef(0.0f, 0.0f, CANDLE_HEIGHT);
    gluDisk(quadric, 0.0, CANDLE_RADIUS, SLICES, 1);

    material_apply(MATERIAL_WICK);
    gluCylinder(quadric, 1.0, 0.6, WICK_HEIGHT, 8, 1);
    glTranslatef(0.0f, 0.0f, WICK_HEIGHT);
    gluDisk(quadric, 0.0, 0.6, 8, 1);

    glPopMatrix();
}

void menorah_draw(int textured)
{
    int i;

    begin_metal(textured);

    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluDisk(quadric, 0.0, BASE_RADIUS, SLICES, 1);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluCylinder(quadric, BASE_RADIUS, BASE_RADIUS * 0.7, BASE_HEIGHT, SLICES, 1);
    glTranslatef(0.0f, 0.0f, BASE_HEIGHT);
    gluDisk(quadric, 0.0, BASE_RADIUS * 0.7, SLICES, 1);
    glPopMatrix();

    draw_post(0.0f, BASE_HEIGHT, CUP_HEIGHT, STEM_RADIUS);
    draw_sphere(0.0f, ARC_CENTER_HEIGHT - 3.0f * ARM_SPACING - 12.0f, KNOB_RADIUS);
    draw_sphere(0.0f, ARC_CENTER_HEIGHT + 10.0f, KNOB_RADIUS * 0.8f);

    for (i = 1; i <= 3; i++) {
        draw_arm(-1.0f, ARM_SPACING * (float)i);
        draw_arm(1.0f, ARM_SPACING * (float)i);
    }
    end_metal();

    for (i = -3; i <= 3; i++) {
        draw_cup_and_candle(ARM_SPACING * (float)i, textured);
    }
}
