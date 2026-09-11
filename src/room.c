#include "room.h"

#include "material.h"

#include <math.h>

#define MAX_FACES 32

typedef struct {
    Quad quad;
    MaterialId material;
} Face;

static Face faces[MAX_FACES];
static int face_count;
static int block_top_face[2];

static void add(MaterialId material, Vec3 c0, Vec3 c1, Vec3 c2, Vec3 c3)
{
    faces[face_count].quad = quad_make(c0, c1, c2, c3);
    faces[face_count].material = material;
    face_count++;
}

static void add_ceiling_piece(float x0, float x1, float z0, float z1)
{
    add(MATERIAL_WHITE,
        v3(x1, ROOM_CEILING_Y, z0),
        v3(x1, ROOM_CEILING_Y, z1),
        v3(x0, ROOM_CEILING_Y, z1),
        v3(x0, ROOM_CEILING_Y, z0));
}

void room_init(void)
{
    face_count = 0;

    add(MATERIAL_WHITE,
        v3(552.8f, 0.0f, 0.0f), v3(0.0f, 0.0f, 0.0f),
        v3(0.0f, 0.0f, 559.2f), v3(549.6f, 0.0f, 559.2f));

    // the ceiling is a frame around the light panel, otherwise they z-fight
    add_ceiling_piece(0.0f, 556.0f, 0.0f, ROOM_LIGHT_MIN_Z);
    add_ceiling_piece(0.0f, 556.0f, ROOM_LIGHT_MAX_Z, 559.2f);
    add_ceiling_piece(0.0f, ROOM_LIGHT_MIN_X, ROOM_LIGHT_MIN_Z, ROOM_LIGHT_MAX_Z);
    add_ceiling_piece(ROOM_LIGHT_MAX_X, 556.0f, ROOM_LIGHT_MIN_Z, ROOM_LIGHT_MAX_Z);

    add(MATERIAL_LIGHT,
        v3(ROOM_LIGHT_MAX_X, ROOM_CEILING_Y, ROOM_LIGHT_MIN_Z),
        v3(ROOM_LIGHT_MAX_X, ROOM_CEILING_Y, ROOM_LIGHT_MAX_Z),
        v3(ROOM_LIGHT_MIN_X, ROOM_CEILING_Y, ROOM_LIGHT_MAX_Z),
        v3(ROOM_LIGHT_MIN_X, ROOM_CEILING_Y, ROOM_LIGHT_MIN_Z));

    add(MATERIAL_WHITE,
        v3(549.6f, 0.0f, 559.2f), v3(0.0f, 0.0f, 559.2f),
        v3(0.0f, 548.8f, 559.2f), v3(556.0f, 548.8f, 559.2f));

    add(MATERIAL_GREEN,
        v3(0.0f, 0.0f, 559.2f), v3(0.0f, 0.0f, 0.0f),
        v3(0.0f, 548.8f, 0.0f), v3(0.0f, 548.8f, 559.2f));

    add(MATERIAL_RED,
        v3(552.8f, 0.0f, 0.0f), v3(549.6f, 0.0f, 559.2f),
        v3(556.0f, 548.8f, 559.2f), v3(556.0f, 548.8f, 0.0f));

    block_top_face[ROOM_SHORT_BLOCK] = face_count;
    add(MATERIAL_WHITE,
        v3(130.0f, 165.0f, 65.0f), v3(82.0f, 165.0f, 225.0f),
        v3(240.0f, 165.0f, 272.0f), v3(290.0f, 165.0f, 114.0f));
    add(MATERIAL_WHITE,
        v3(290.0f, 0.0f, 114.0f), v3(290.0f, 165.0f, 114.0f),
        v3(240.0f, 165.0f, 272.0f), v3(240.0f, 0.0f, 272.0f));
    add(MATERIAL_WHITE,
        v3(130.0f, 0.0f, 65.0f), v3(130.0f, 165.0f, 65.0f),
        v3(290.0f, 165.0f, 114.0f), v3(290.0f, 0.0f, 114.0f));
    add(MATERIAL_WHITE,
        v3(82.0f, 0.0f, 225.0f), v3(82.0f, 165.0f, 225.0f),
        v3(130.0f, 165.0f, 65.0f), v3(130.0f, 0.0f, 65.0f));
    add(MATERIAL_WHITE,
        v3(240.0f, 0.0f, 272.0f), v3(240.0f, 165.0f, 272.0f),
        v3(82.0f, 165.0f, 225.0f), v3(82.0f, 0.0f, 225.0f));

    block_top_face[ROOM_TALL_BLOCK] = face_count;
    add(MATERIAL_WHITE,
        v3(423.0f, 330.0f, 247.0f), v3(265.0f, 330.0f, 296.0f),
        v3(314.0f, 330.0f, 456.0f), v3(472.0f, 330.0f, 406.0f));
    add(MATERIAL_WHITE,
        v3(423.0f, 0.0f, 247.0f), v3(423.0f, 330.0f, 247.0f),
        v3(472.0f, 330.0f, 406.0f), v3(472.0f, 0.0f, 406.0f));
    add(MATERIAL_WHITE,
        v3(472.0f, 0.0f, 406.0f), v3(472.0f, 330.0f, 406.0f),
        v3(314.0f, 330.0f, 456.0f), v3(314.0f, 0.0f, 456.0f));
    add(MATERIAL_WHITE,
        v3(314.0f, 0.0f, 456.0f), v3(314.0f, 330.0f, 456.0f),
        v3(265.0f, 330.0f, 296.0f), v3(265.0f, 0.0f, 296.0f));
    add(MATERIAL_WHITE,
        v3(265.0f, 0.0f, 296.0f), v3(265.0f, 330.0f, 296.0f),
        v3(423.0f, 330.0f, 247.0f), v3(423.0f, 0.0f, 247.0f));
}

int room_face_count(void)
{
    return face_count;
}

const Quad *room_face(int index)
{
    return &faces[index].quad;
}

MaterialId room_face_material(int index)
{
    return faces[index].material;
}

Vec3 room_block_top_center(RoomBlock block)
{
    return quad_point(&faces[block_top_face[block]].quad, 0.5f, 0.5f);
}

float room_block_yaw_degrees(RoomBlock block)
{
    const Quad *top = &faces[block_top_face[block]].quad;
    int nearest = 0;
    float yaw;
    int i;

    for (i = 1; i < 4; i++) {
        const float z = top->corner[i].z + top->corner[(i + 1) % 4].z;

        if (z < top->corner[nearest].z + top->corner[(nearest + 1) % 4].z) {
            nearest = i;
        }
    }

    {
        const Vec3 edge = v3_sub(top->corner[(nearest + 1) % 4], top->corner[nearest]);

        yaw = -atan2f(edge.z, edge.x) * 180.0f / VEC_PI;
    }

    while (yaw <= -90.0f) {
        yaw += 180.0f;
    }
    while (yaw > 90.0f) {
        yaw -= 180.0f;
    }

    return yaw;
}
