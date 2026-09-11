#include "camera.h"
#include "check.h"
#include "light.h"
#include "props/dreidel.h"
#include "props/hebrew_glyphs.h"
#include "props/magendavid.h"
#include "quad.h"
#include "room.h"
#include "shadow.h"
#include "vecmath.h"

#include <math.h>
#include <stdio.h>

int failures;

void check(int condition, const char *what)
{
    if (!condition) {
        printf("FAIL %s\n", what);
        failures++;
    }
}

void check_close(float actual, float expected, float tolerance, const char *what)
{
    if (fabsf(actual - expected) > tolerance) {
        printf("FAIL %s: got %.6f, want %.6f\n", what, actual, expected);
        failures++;
    }
}

static float axis(Vec3 v, int index)
{
    return index == 0 ? v.x : (index == 1 ? v.y : v.z);
}

static void test_vecmath(void)
{
    const Vec3 n = v3_cross(v3(1.0f, 0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f));

    check_close(n.x, 0.0f, 1e-6f, "cross x");
    check_close(n.y, -1.0f, 1e-6f, "cross y");
    check_close(n.z, 0.0f, 1e-6f, "cross z");
    check_close(v3_length(v3_normalize(v3(3.0f, 4.0f, 12.0f))), 1.0f, 1e-6f, "normalize length");
    check_close(v3_length(v3(3.0f, 4.0f, 12.0f)), 13.0f, 1e-5f, "length");
}

static void test_quad_patch(void)
{
    const Quad unit = quad_make(v3(0.0f, 0.0f, 0.0f), v3(1.0f, 0.0f, 0.0f),
                                v3(1.0f, 0.0f, 1.0f), v3(0.0f, 0.0f, 1.0f));
    const int steps = 7;
    float subdivided = 0.0f;
    int i;
    int j;

    check_close(quad_point(&unit, 0.0f, 0.0f).x, 0.0f, 1e-6f, "corner 0 maps to (0,0)");
    check_close(quad_point(&unit, 1.0f, 0.0f).x, 1.0f, 1e-6f, "corner 1 maps to (1,0)");
    check_close(quad_point(&unit, 1.0f, 1.0f).z, 1.0f, 1e-6f, "corner 2 maps to (1,1)");
    check_close(quad_point(&unit, 0.0f, 1.0f).z, 1.0f, 1e-6f, "corner 3 maps to (0,1)");
    check_close(quad_area(&unit), 1.0f, 1e-6f, "unit quad area");

    for (j = 0; j < steps; j++) {
        for (i = 0; i < steps; i++) {
            const float u0 = (float)i / (float)steps;
            const float u1 = (float)(i + 1) / (float)steps;
            const float v0 = (float)j / (float)steps;
            const float v1 = (float)(j + 1) / (float)steps;
            const Quad cell = quad_make(quad_point(&unit, u0, v0), quad_point(&unit, u1, v0),
                                        quad_point(&unit, u1, v1), quad_point(&unit, u0, v1));

            subdivided += quad_area(&cell);
        }
    }

    check_close(subdivided, quad_area(&unit), 1e-4f, "subdivision preserves area");
}

static const struct {
    const char *name;
    int axis;
    float sign;
} expected_normals[] = {
    {"floor", 1, 1.0f},
    {"ceiling front", 1, -1.0f},
    {"ceiling back", 1, -1.0f},
    {"ceiling left", 1, -1.0f},
    {"ceiling right", 1, -1.0f},
    {"light", 1, -1.0f},
    {"back wall", 2, -1.0f},
    {"green wall", 0, 1.0f},
    {"red wall", 0, -1.0f},
    {"short block top", 1, 1.0f},
    {"short block side a", 0, 1.0f},
    {"short block side b", 2, -1.0f},
    {"short block side c", 0, -1.0f},
    {"short block side d", 2, 1.0f},
    {"tall block top", 1, 1.0f},
    {"tall block side a", 0, 1.0f},
    {"tall block side b", 2, 1.0f},
    {"tall block side c", 0, -1.0f},
    {"tall block side d", 2, -1.0f}
};

static void test_room_normals(void)
{
    const int count = (int)(sizeof expected_normals / sizeof expected_normals[0]);
    int i;

    check(room_face_count() == count, "room face count matches the table");

    for (i = 0; i < count && i < room_face_count(); i++) {
        const Vec3 n = quad_normal(room_face(i), 0.5f, 0.5f);
        const float dominant = axis(n, expected_normals[i].axis);

        check_close(v3_length(n), 1.0f, 1e-5f, expected_normals[i].name);
        check(dominant * expected_normals[i].sign > 0.9f, expected_normals[i].name);
    }
}

static void test_red_wall_is_a_patch(void)
{
    const Quad *red = room_face(8);
    const Vec3 near_corner = quad_normal(red, 0.0f, 0.0f);
    const Vec3 far_corner = quad_normal(red, 1.0f, 1.0f);
    const float alignment = v3_dot(near_corner, far_corner);

    check(alignment < 0.99999f, "red wall normal varies");
    check(alignment > 0.999f, "red wall stays nearly flat");
}

static void check_samples_cover_panel(Vec3 (*sample)(int), int count, const char *what)
{
    Vec3 centroid = v3(0.0f, 0.0f, 0.0f);
    int i;

    for (i = 0; i < count; i++) {
        const Vec3 point = sample(i);

        check(point.x > ROOM_LIGHT_MIN_X && point.x < ROOM_LIGHT_MAX_X, what);
        check(point.z > ROOM_LIGHT_MIN_Z && point.z < ROOM_LIGHT_MAX_Z, what);
        check_close(point.y, ROOM_CEILING_Y, 1e-6f, what);

        centroid = v3_add(centroid, v3_scale(point, 1.0f / (float)count));
    }

    check_close(centroid.x, 0.5f * (ROOM_LIGHT_MIN_X + ROOM_LIGHT_MAX_X), 1e-3f, what);
    check_close(centroid.z, 0.5f * (ROOM_LIGHT_MIN_Z + ROOM_LIGHT_MAX_Z), 1e-3f, what);
}

static void test_light_samples(void)
{
    check_samples_cover_panel(light_panel_sample, LIGHT_PANEL_SAMPLE_COUNT, "lighting samples");
    check_samples_cover_panel(shadow_sample_position, shadow_sample_count(), "shadow samples");
    check(shadow_sample_count() > LIGHT_PANEL_SAMPLE_COUNT, "shadow grid is finer than the light grid");
}

static void test_bar_faces_point_outwards(void)
{
    const Vec3 a = v3(10.0f, 20.0f, 0.0f);
    const Vec3 b = v3(70.0f, 60.0f, 0.0f);
    const Vec3 center = v3_scale(v3_add(a, b), 0.5f);
    Quad faces[6];
    int i;

    magendavid_bar_quads(a, b, 6.0f, v3(0.0f, 0.0f, 1.0f), faces);

    for (i = 0; i < 6; i++) {
        const Vec3 face_center = quad_point(&faces[i], 0.5f, 0.5f);
        const Vec3 outward = v3_sub(face_center, center);

        check(v3_dot(quad_normal(&faces[i], 0.5f, 0.5f), outward) > 0.0f, "bar face normal points outwards");
        check_close(quad_area(&faces[i]), i < 2 ? 36.0f : 6.0f * v3_length(v3_sub(b, a)), 1e-2f,
                    "bar face area");
    }
}

static void check_faces_outwards(const Vec3 *center, const Quad *face, const char *what)
{
    const Vec3 outward = v3_sub(quad_point(face, 0.5f, 0.5f), *center);

    check(v3_dot(quad_normal(face, 0.5f, 0.5f), outward) > 0.0f, what);
}

static void test_dreidel_is_closed_outwards(void)
{
    const Vec3 center = v3(0.0f, 35.0f, 0.0f);
    DreidelBody body;
    int i;

    dreidel_body(&body);

    for (i = 0; i < 4; i++) {
        const Vec3 *tri = body.tip[i];
        const Vec3 normal = v3_cross(v3_sub(tri[1], tri[0]), v3_sub(tri[2], tri[0]));
        const Vec3 tri_center = v3_scale(v3_add(v3_add(tri[0], tri[1]), tri[2]), 1.0f / 3.0f);

        check_faces_outwards(&center, &body.side[i], "dreidel side faces outwards");
        check(v3_dot(normal, v3_sub(tri_center, center)) > 0.0f, "dreidel tip face faces outwards");
        check(tri[0].y == 0.0f, "dreidel tip faces meet at the point");
    }
    check_faces_outwards(&center, &body.top, "dreidel top faces up");
}

static void test_block_placement(void)
{
    const Vec3 short_top = room_block_top_center(ROOM_SHORT_BLOCK);
    const Vec3 tall_top = room_block_top_center(ROOM_TALL_BLOCK);

    check_close(short_top.y, 165.0f, 1e-4f, "short block top height");
    check_close(tall_top.y, 330.0f, 1e-4f, "tall block top height");
    check_close(short_top.x, 185.5f, 1e-3f, "short block top center x");
    check_close(tall_top.z, 351.25f, 1e-3f, "tall block top center z");
    check_close(room_block_yaw_degrees(ROOM_SHORT_BLOCK), -17.03f, 0.05f, "short block yaw");
    check_close(room_block_yaw_degrees(ROOM_TALL_BLOCK), 17.23f, 0.05f, "tall block yaw");
}

static void test_glyph_bitmaps(void)
{
    static unsigned char pixels[HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE];
    int glyph;

    for (glyph = 0; glyph < HEBREW_GLYPH_COUNT; glyph++) {
        int ink = 0;
        int on_border = 0;
        int i;

        hebrew_glyph_unpack(glyph, 1, 0, pixels);
        for (i = 0; i < HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE; i++) {
            const int x = i % HEBREW_GLYPH_SIZE;
            const int y = i / HEBREW_GLYPH_SIZE;

            ink += pixels[i];
            if (pixels[i] && (x < 4 || y < 4 || x >= HEBREW_GLYPH_SIZE - 4 || y >= HEBREW_GLYPH_SIZE - 4)) {
                on_border++;
            }
        }

        check(ink > HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE / 20, "glyph has ink");
        check(ink < HEBREW_GLYPH_SIZE * HEBREW_GLYPH_SIZE / 2, "glyph is not a blob");
        check(on_border == 0, "glyph margin is clear");
    }
}

static void test_camera(void)
{
    Vec3 eye;

    camera_reset();
    eye = camera_eye();
    check_close(eye.x, 278.0f, 1e-3f, "rest eye x");
    check_close(eye.y, 273.0f, 1e-3f, "rest eye y");
    check_close(eye.z, -800.0f, 1e-3f, "rest eye z");

    camera_orbit(90.0f, 0.0f);
    eye = camera_eye();
    check(eye.x > 1000.0f, "yaw 90 swings the eye to +x");
    check_close(eye.z, 279.6f, 1e-2f, "yaw 90 keeps z at the pivot");

    camera_reset();
    camera_orbit(0.0f, 200.0f);
    eye = camera_eye();
    check(eye.y < 273.0f + 1079.6f, "pitch clamped below 90");
    check(eye.y > 273.0f + 1000.0f, "pitch clamp is near the top");

    camera_reset();
    camera_zoom(0.001f);
    check_close(v3_length(v3_sub(camera_eye(), v3(278.0f, 273.0f, 279.6f))), 150.0f, 1e-2f,
                "zoom clamps at the minimum distance");
    camera_reset();
}

void test_app_main(void);

int main(void)
{
    room_init();

    test_vecmath();
    test_quad_patch();
    test_room_normals();
    test_red_wall_is_a_patch();
    test_light_samples();
    test_bar_faces_point_outwards();
    test_dreidel_is_closed_outwards();
    test_block_placement();
    test_glyph_bitmaps();
    test_camera();
    test_app_main();

    if (failures > 0) {
        printf("%d check(s) failed\n", failures);
        return 1;
    }

    printf("all checks passed\n");
    return 0;
}
