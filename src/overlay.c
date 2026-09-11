#include "overlay.h"

#include "gl_compat.h"

#include <stdio.h>

#define LINE_HEIGHT 16
#define MARGIN 12

static void draw_text(int x, int y, const char *text)
{
    const char *c;

    glRasterPos2i(x, y);
    for (c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

static void draw_line(int line, const char *text)
{
    const int y = MARGIN + LINE_HEIGHT * (3 - line);

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text(MARGIN + 1, y - 1, text);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_text(MARGIN, y, text);
}

static const char *state(int on)
{
    return on ? "on" : "off";
}

void overlay_draw(const AppState *app, int shadows_available, int width, int height)
{
    char line[128];

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    draw_line(0, "Cornell Box");
    snprintf(line, sizeof line, "1 subdivision: %d   2 textures: %s   3 shadows: %s",
             app->subdivision_steps, state(app->textures),
             shadows_available ? state(app->shadows) : "unavailable on this GPU");
    draw_line(1, line);
    snprintf(line, sizeof line, "4 antialiasing: %s   5 transparency: %s   F fill lights: %s",
             state(app->antialiasing), state(app->transparency), state(app->fill_lights));
    draw_line(2, line);
    draw_line(3, "mouse drag or W/A/S/D orbit, +/- zoom, R reset view, C screenshot, H hide help, Q quit");

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
