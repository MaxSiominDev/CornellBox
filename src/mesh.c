#include "mesh.h"

#include "gl_compat.h"

static void emit_vertex(const Quad *quad, float u, float v, int textured)
{
    const Vec3 position = quad_point(quad, u, v);
    const Vec3 normal = quad_normal(quad, u, v);

    if (textured) {
        glTexCoord2f(u, v);
    }
    glNormal3f(normal.x, normal.y, normal.z);
    glVertex3f(position.x, position.y, position.z);
}

static void draw(const Quad *quad, int steps, int textured)
{
    int i;
    int j;

    glBegin(GL_QUADS);
    for (j = 0; j < steps; j++) {
        const float v0 = (float)j / (float)steps;
        const float v1 = (float)(j + 1) / (float)steps;

        for (i = 0; i < steps; i++) {
            const float u0 = (float)i / (float)steps;
            const float u1 = (float)(i + 1) / (float)steps;

            emit_vertex(quad, u0, v0, textured);
            emit_vertex(quad, u1, v0, textured);
            emit_vertex(quad, u1, v1, textured);
            emit_vertex(quad, u0, v1, textured);
        }
    }
    glEnd();
}

void mesh_draw_quad(const Quad *quad, int steps)
{
    draw(quad, steps, 0);
}

void mesh_draw_quad_textured(const Quad *quad, int steps)
{
    draw(quad, steps, 1);
}
