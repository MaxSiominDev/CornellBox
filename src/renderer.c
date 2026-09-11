#include "renderer.h"

#include "camera.h"
#include "gl_compat.h"
#include "light.h"
#include "overlay.h"
#include "scene.h"
#include "shadow.h"

#include <stdio.h>

#define DEPTH_PASS_STEPS 1

static int shadows_available;

static void draw_casters(void *context)
{
    (void)context;
    scene_draw_surfaces(DEPTH_PASS_STEPS, 0);
    scene_draw_glass();
}

static void draw_lit(const AppState *app)
{
    scene_draw_surfaces(app->subdivision_steps, app->textures);
    if (!app->transparency) {
        scene_draw_glass();
    }
}

void renderer_init(void)
{
    const char *reason = NULL;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);

    scene_init();

    shadows_available = shadow_init(&reason) == 0;
    if (!shadows_available) {
        fprintf(stderr, "shadows unavailable: %s\n", reason);
    }
}

static void begin_additive(void)
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
}

static void end_additive(void)
{
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

static void add_shadowed_panel(const AppState *app)
{
    const int count = shadow_sample_count();
    int i;

    for (i = 0; i < count; i++) {
        light_apply_panel_point(shadow_sample_position(i), 1.0f / (float)count);
        shadow_bind(i);
        draw_lit(app);
        shadow_unbind();
    }
}

static void draw_glass_layer(const AppState *app)
{
    light_apply_base(app->fill_lights);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    scene_draw_glass();

    light_apply_panel_grid();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    scene_draw_glass();
}

static void draw_transparent(const AppState *app)
{
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    glCullFace(GL_FRONT);
    draw_glass_layer(app);
    glCullFace(GL_BACK);
    draw_glass_layer(app);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void renderer_draw_frame(const AppState *app, int width, int height)
{
    const int shadowed = app->shadows && shadows_available;
    int i;

    if (shadowed) {
        for (i = 0; i < shadow_sample_count(); i++) {
            shadow_render_depth(i, draw_casters, NULL, width, height);
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (app->antialiasing) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }

    camera_apply_projection(width, height);
    camera_apply_view();

    light_apply_base(app->fill_lights);
    draw_lit(app);
    scene_draw_emitters();

    begin_additive();
    if (shadowed) {
        add_shadowed_panel(app);
    } else {
        light_apply_panel_grid();
        draw_lit(app);
    }
    end_additive();

    if (app->transparency) {
        draw_transparent(app);
    }

    if (app->help) {
        overlay_draw(app, shadows_available, width, height);
    }
}
