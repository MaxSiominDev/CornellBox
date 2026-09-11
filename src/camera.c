#include "camera.h"

#include "gl_compat.h"

#include <math.h>

#define CAMERA_FOV_DEGREES 39.3076f
#define CAMERA_NEAR 10.0f
#define CAMERA_FAR 4000.0f

#define PIVOT_X 278.0f
#define PIVOT_Y 273.0f
#define PIVOT_Z 279.6f
#define REST_DISTANCE 1079.6f
#define MIN_DISTANCE 150.0f
#define MAX_DISTANCE 3000.0f
#define MAX_PITCH 85.0f

static float yaw;
static float pitch;
static float distance = REST_DISTANCE;

void camera_reset(void)
{
    yaw = 0.0f;
    pitch = 0.0f;
    distance = REST_DISTANCE;
}

void camera_orbit(float yaw_degrees, float pitch_degrees)
{
    yaw += yaw_degrees;
    pitch += pitch_degrees;
    if (pitch > MAX_PITCH) {
        pitch = MAX_PITCH;
    }
    if (pitch < -MAX_PITCH) {
        pitch = -MAX_PITCH;
    }
}

void camera_zoom(float factor)
{
    distance *= factor;
    if (distance < MIN_DISTANCE) {
        distance = MIN_DISTANCE;
    }
    if (distance > MAX_DISTANCE) {
        distance = MAX_DISTANCE;
    }
}

Vec3 camera_eye(void)
{
    const float yaw_radians = yaw * VEC_PI / 180.0f;
    const float pitch_radians = pitch * VEC_PI / 180.0f;
    const float flat = distance * cosf(pitch_radians);

    return v3(PIVOT_X + flat * sinf(yaw_radians),
              PIVOT_Y + distance * sinf(pitch_radians),
              PIVOT_Z - flat * cosf(yaw_radians));
}

void camera_apply_projection(int width, int height)
{
    const double aspect = height > 0 ? (double)width / (double)height : 1.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(CAMERA_FOV_DEGREES, aspect, CAMERA_NEAR, CAMERA_FAR);
}

void camera_apply_view(void)
{
    const Vec3 eye = camera_eye();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, PIVOT_X, PIVOT_Y, PIVOT_Z, 0.0, 1.0, 0.0);
}
