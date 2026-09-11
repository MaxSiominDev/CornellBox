#include "scene.h"

#include "gl_compat.h"
#include "material.h"
#include "mesh.h"
#include "props/dreidel.h"
#include "props/magendavid.h"
#include "props/menorah.h"
#include "room.h"

#define DREIDEL_X 400.0f
#define DREIDEL_Z 120.0f
#define DREIDEL_TILT_DEGREES 8.0f
#define DREIDEL_YAW_DEGREES -20.0f

#define LIST_CACHE_SIZE 6

typedef struct {
    int steps;
    int textured;
    GLuint list;
} SurfaceList;

static SurfaceList cache[LIST_CACHE_SIZE];
static int cache_count;
static GLuint emitters_list;
static GLuint glass_list;

static void push_block_frame(RoomBlock block)
{
    const Vec3 top = room_block_top_center(block);

    glPushMatrix();
    glTranslatef(top.x, top.y, top.z);
    glRotatef(room_block_yaw_degrees(block), 0.0f, 1.0f, 0.0f);
}

static void draw_opaque_props(int textured)
{
    push_block_frame(ROOM_SHORT_BLOCK);
    menorah_draw(textured);
    glPopMatrix();

    push_block_frame(ROOM_TALL_BLOCK);
    magendavid_draw_pedestal();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(DREIDEL_X, 0.0f, DREIDEL_Z);
    glRotatef(DREIDEL_YAW_DEGREES, 0.0f, 1.0f, 0.0f);
    glRotatef(DREIDEL_TILT_DEGREES, 0.0f, 0.0f, 1.0f);
    dreidel_draw(textured);
    glPopMatrix();
}

static void draw_room_faces(int steps, int emitters)
{
    const int count = room_face_count();
    int i;

    for (i = 0; i < count; i++) {
        const MaterialId material = room_face_material(i);

        if ((material == MATERIAL_LIGHT) != emitters) {
            continue;
        }
        material_apply(material);
        mesh_draw_quad(room_face(i), emitters ? 1 : steps);
    }
}

static GLuint surfaces_for(int steps, int textured)
{
    SurfaceList *entry;
    int i;

    for (i = 0; i < cache_count; i++) {
        if (cache[i].steps == steps && cache[i].textured == textured) {
            return cache[i].list;
        }
    }

    if (cache_count < LIST_CACHE_SIZE) {
        entry = &cache[cache_count++];
    } else {
        entry = &cache[0];
        glDeleteLists(entry->list, 1);
    }

    entry->steps = steps;
    entry->textured = textured;
    entry->list = glGenLists(1);
    glNewList(entry->list, GL_COMPILE);
    draw_room_faces(steps, 0);
    draw_opaque_props(textured);
    glEndList();

    return entry->list;
}

void scene_init(void)
{
    room_init();
    menorah_init();
    magendavid_init();
    dreidel_init();
}

void scene_draw_surfaces(int steps, int textured)
{
    glCallList(surfaces_for(steps, textured));
}

void scene_draw_emitters(void)
{
    if (emitters_list == 0) {
        emitters_list = glGenLists(1);
        glNewList(emitters_list, GL_COMPILE);
        draw_room_faces(1, 1);
        glEndList();
    }
    glCallList(emitters_list);
}

void scene_draw_glass(void)
{
    if (glass_list == 0) {
        glass_list = glGenLists(1);
        glNewList(glass_list, GL_COMPILE);
        push_block_frame(ROOM_TALL_BLOCK);
        magendavid_draw_star();
        glPopMatrix();
        glEndList();
    }
    glCallList(glass_list);
}
