#ifndef ROOM_H
#define ROOM_H

#include "material.h"
#include "quad.h"

#define ROOM_LIGHT_MIN_X 213.0f
#define ROOM_LIGHT_MAX_X 343.0f
#define ROOM_LIGHT_MIN_Z 227.0f
#define ROOM_LIGHT_MAX_Z 332.0f
#define ROOM_CEILING_Y 548.8f

void room_init(void);
int room_face_count(void);
const Quad *room_face(int index);
MaterialId room_face_material(int index);

typedef enum {
    ROOM_SHORT_BLOCK,
    ROOM_TALL_BLOCK
} RoomBlock;

Vec3 room_block_top_center(RoomBlock block);
float room_block_yaw_degrees(RoomBlock block);

#endif
