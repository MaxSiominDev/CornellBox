#include "light.h"

#include "gl_compat.h"
#include "room.h"

#define PANEL_INTENSITY 1.1f
#define PANEL_REFERENCE_DISTANCE 1400.0f
#define FILL_INTENSITY 1.5f
#define FILL_TINT 0.45f
#define AMBIENT_LEVEL 0.05f
#define MAX_LIGHTS 8

static const struct {
    Vec3 towards;
    Vec3 color;
} fill_lights[] = {
    {{0.0f, -1.0f, 0.0f}, {0.725f, 0.710f, 0.680f}},
    {{0.0f, -0.35f, -1.0f}, {0.725f, 0.710f, 0.680f}},
    {{0.0f, 1.0f, 0.0f}, {0.725f, 0.710f, 0.680f}},
    {{1.0f, 0.0f, 0.0f}, {0.630f, 0.065f, 0.050f}},
    {{-1.0f, 0.0f, 0.0f}, {0.140f, 0.450f, 0.091f}}
};

#define FILL_LIGHT_COUNT ((int)(sizeof fill_lights / sizeof fill_lights[0]))

static const Vec3 panel_color = {1.0f, 0.97f, 0.90f};
static const Vec3 neutral_bounce = {0.725f, 0.710f, 0.680f};

static void configure(int slot, Vec3 position, float w, Vec3 color, float quadratic, int specular)
{
    const GLfloat position4[4] = {position.x, position.y, position.z, w};
    const GLfloat diffuse[4] = {color.x, color.y, color.z, 1.0f};
    const GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0 + slot, GL_POSITION, position4);
    glLightfv(GL_LIGHT0 + slot, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0 + slot, GL_SPECULAR, specular ? diffuse : black);
    glLightfv(GL_LIGHT0 + slot, GL_AMBIENT, black);
    glLightf(GL_LIGHT0 + slot, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0 + slot, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0 + slot, GL_QUADRATIC_ATTENUATION, quadratic);
    glEnable(GL_LIGHT0 + slot);
}

static void disable_from(int slot)
{
    for (; slot < MAX_LIGHTS; slot++) {
        glDisable(GL_LIGHT0 + slot);
    }
}

static void set_ambient(float level)
{
    const GLfloat ambient[4] = {level, level, level, 1.0f};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

static void configure_panel_light(int slot, Vec3 position, float share)
{
    const float falloff = 1.0f / (PANEL_REFERENCE_DISTANCE * PANEL_REFERENCE_DISTANCE);

    configure(slot, position, 1.0f, v3_scale(panel_color, PANEL_INTENSITY * share), falloff, 1);
}

static void configure_fills(int first_slot, int fill_enabled)
{
    const float share = FILL_INTENSITY / (float)FILL_LIGHT_COUNT;
    int i;

    if (!fill_enabled) {
        disable_from(first_slot);
        return;
    }

    for (i = 0; i < FILL_LIGHT_COUNT; i++) {
        const Vec3 tinted = v3_lerp(neutral_bounce, fill_lights[i].color, FILL_TINT);

        configure(first_slot + i, v3_normalize(fill_lights[i].towards), 0.0f,
                  v3_scale(tinted, share), 0.0f, 0);
    }
    disable_from(first_slot + FILL_LIGHT_COUNT);
}

Vec3 light_panel_grid_point(int index, int columns, int rows)
{
    const float u = ((float)(index % columns) + 0.5f) / (float)columns;
    const float v = ((float)(index / columns) + 0.5f) / (float)rows;

    return v3(ROOM_LIGHT_MIN_X + u * (ROOM_LIGHT_MAX_X - ROOM_LIGHT_MIN_X),
              ROOM_CEILING_Y,
              ROOM_LIGHT_MIN_Z + v * (ROOM_LIGHT_MAX_Z - ROOM_LIGHT_MIN_Z));
}

Vec3 light_panel_sample(int index)
{
    return light_panel_grid_point(index, LIGHT_PANEL_SAMPLES_X, LIGHT_PANEL_SAMPLES_Z);
}

void light_apply_base(int fill_enabled)
{
    set_ambient(AMBIENT_LEVEL);
    configure_fills(0, fill_enabled);
}

void light_apply_panel_grid(void)
{
    int i;

    set_ambient(0.0f);
    for (i = 0; i < LIGHT_PANEL_SAMPLE_COUNT; i++) {
        configure_panel_light(i, light_panel_sample(i), 1.0f / (float)LIGHT_PANEL_SAMPLE_COUNT);
    }
    disable_from(LIGHT_PANEL_SAMPLE_COUNT);
}

void light_apply_panel_point(Vec3 position, float share)
{
    set_ambient(0.0f);
    configure_panel_light(0, position, share);
    disable_from(1);
}
