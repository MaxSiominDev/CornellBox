#include "dreidel.h"

#include "gl_compat.h"
#include "hebrew_glyphs.h"
#include "material.h"
#include "mesh.h"
#include "texture.h"

#define SLICES 16
#define HALF_WIDTH 16.0f
#define TIP_HEIGHT 18.0f
#define BODY_HEIGHT 34.0f
#define HANDLE_RADIUS 5.0f
#define HANDLE_HEIGHT 20.0f
#define INK 70
#define PAPER 255

static GLUquadric *quadric;
static GLuint letter_textures[HEBREW_GLYPH_COUNT];

void dreidel_body(DreidelBody *out)
{
    const float top = TIP_HEIGHT + BODY_HEIGHT;
    const Vec3 tip = v3(0.0f, 0.0f, 0.0f);
    const Vec3 lower[4] = {
        v3(-HALF_WIDTH, TIP_HEIGHT, -HALF_WIDTH), v3(-HALF_WIDTH, TIP_HEIGHT, HALF_WIDTH),
        v3(HALF_WIDTH, TIP_HEIGHT, HALF_WIDTH), v3(HALF_WIDTH, TIP_HEIGHT, -HALF_WIDTH)
    };
    Vec3 upper[4];
    int i;

    for (i = 0; i < 4; i++) {
        upper[i] = v3(lower[i].x, top, lower[i].z);
    }

    for (i = 0; i < 4; i++) {
        const int next = (i + 1) % 4;

        out->side[i] = quad_make(lower[i], lower[next], upper[next], upper[i]);
        out->tip[i][0] = tip;
        out->tip[i][1] = lower[next];
        out->tip[i][2] = lower[i];
    }
    out->top = quad_make(upper[0], upper[1], upper[2], upper[3]);
}

void dreidel_init(void)
{
    unsigned char pixels[HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE];
    int glyph;

    for (glyph = 0; glyph < HEBREW_GLYPH_COUNT; glyph++) {
        hebrew_glyph_unpack(glyph, INK, PAPER, pixels);
        letter_textures[glyph] = texture_from_luminance(pixels, HEBREW_GLYPH_SIZE, HEBREW_GLYPH_SIZE);
    }

    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
}

static void draw_lettered_side(const Quad *side, int glyph)
{
    glBindTexture(GL_TEXTURE_2D, letter_textures[glyph]);
    glEnable(GL_TEXTURE_2D);
    mesh_draw_quad_textured(side, 1);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void draw_triangle(const Vec3 *corner)
{
    const Vec3 normal = v3_normalize(v3_cross(v3_sub(corner[1], corner[0]), v3_sub(corner[2], corner[0])));
    int i;

    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x, normal.y, normal.z);
    for (i = 0; i < 3; i++) {
        glVertex3f(corner[i].x, corner[i].y, corner[i].z);
    }
    glEnd();
}

void dreidel_draw(int textured)
{
    DreidelBody body;
    int i;

    dreidel_body(&body);

    material_apply(MATERIAL_WOOD);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    for (i = 0; i < 4; i++) {
        if (textured) {
            draw_lettered_side(&body.side[i], i);
        } else {
            mesh_draw_quad(&body.side[i], 1);
        }
        draw_triangle(body.tip[i]);
    }
    mesh_draw_quad(&body.top, 1);

    glPushMatrix();
    glTranslatef(0.0f, TIP_HEIGHT + BODY_HEIGHT, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, HANDLE_RADIUS, HANDLE_RADIUS, HANDLE_HEIGHT, SLICES, 1);
    glTranslatef(0.0f, 0.0f, HANDLE_HEIGHT);
    gluDisk(quadric, 0.0, HANDLE_RADIUS, SLICES, 1);
    glPopMatrix();
}
