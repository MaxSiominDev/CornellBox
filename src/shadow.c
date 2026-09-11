#include "shadow.h"

#include "gl_ext.h"
#include "light.h"

#include <stddef.h>

#define SHADOW_MAP_SIZE 1024
#define SHADOW_SAMPLES_X 4
#define SHADOW_SAMPLES_Z 4
#define SHADOW_SAMPLE_COUNT (SHADOW_SAMPLES_X * SHADOW_SAMPLES_Z)
#define SHADOW_FOV_DEGREES 140.0
#define SHADOW_NEAR 20.0
#define SHADOW_FAR 1000.0

static GLuint depth_textures[SHADOW_SAMPLE_COUNT];
static GLuint framebuffers[SHADOW_SAMPLE_COUNT];

int shadow_sample_count(void)
{
    return SHADOW_SAMPLE_COUNT;
}

Vec3 shadow_sample_position(int sample)
{
    return light_panel_grid_point(sample, SHADOW_SAMPLES_X, SHADOW_SAMPLES_Z);
}

static void load_light_view(int sample)
{
    const Vec3 eye = shadow_sample_position(sample);

    gluLookAt(eye.x, eye.y, eye.z,
              eye.x, eye.y - 1.0f, eye.z,
              0.0, 0.0, 1.0);
}

static int create_map(int sample)
{
    const GLfloat lit_border[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glGenTextures(1, &depth_textures[sample]);
    glBindTexture(GL_TEXTURE_2D, depth_textures[sample]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,
                 GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, lit_border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffersEXT(1, &framebuffers[sample]);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffers[sample]);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D,
                              depth_textures[sample], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    return glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT ? 0 : -1;
}

int shadow_init(const char **reason)
{
    int i;

    if (gl_ext_load(reason) != 0) {
        return -1;
    }

    for (i = 0; i < SHADOW_SAMPLE_COUNT; i++) {
        if (create_map(i) != 0) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            *reason = "depth-only framebuffer is incomplete";
            return -1;
        }
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);

    return 0;
}

void shadow_render_depth(int sample, void (*draw)(void *), void *context, int width, int height)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffers[sample]);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(SHADOW_FOV_DEGREES, 1.0, SHADOW_NEAR, SHADOW_FAR);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    load_light_view(sample);

    // only back faces go into the map, so no depth bias is needed
    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT);

    draw(context);

    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, width, height);
}

// with the camera view loaded, eye-linear planes = bias * light projection * light view
static void set_projective_planes(int sample)
{
    static const GLenum coordinates[4] = {GL_S, GL_T, GL_R, GL_Q};
    GLfloat matrix[16];
    int row;

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glScalef(0.5f, 0.5f, 0.5f);
    gluPerspective(SHADOW_FOV_DEGREES, 1.0, SHADOW_NEAR, SHADOW_FAR);
    load_light_view(sample);
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();

    for (row = 0; row < 4; row++) {
        const GLfloat plane[4] = {matrix[row], matrix[4 + row], matrix[8 + row], matrix[12 + row]};

        glTexGeni(coordinates[row], GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(coordinates[row], GL_EYE_PLANE, plane);
    }
}

void shadow_bind(int sample)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_textures[sample]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    set_projective_planes(sample);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    glActiveTexture(GL_TEXTURE0);
}

void shadow_unbind(void)
{
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}
