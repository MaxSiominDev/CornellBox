#include "app.h"
#include "camera.h"
#include "gl_compat.h"
#include "renderer.h"
#include "screenshot.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// on macOS the first frames of a new window can read back garbage
#define SCREENSHOT_WARMUP_FRAMES 3

static AppState app;
static const char *screenshot_path = NULL;
static const char *scripted_keys = "";
static int window_size = 800;
static int viewport_width = 0;
static int viewport_height = 0;
static int frames_rendered = 0;
static int capture_requested = 0;
static int captures_taken = 0;
static int bench_frames = 0;
static int bench_started_at = 0;
static int drag_button = -1;
static int drag_x = 0;
static int drag_y = 0;

#define ORBIT_STEP_DEGREES 5.0f
#define ORBIT_DEGREES_PER_PIXEL 0.4f
#define ZOOM_STEP 1.1f
#define ZOOM_PER_PIXEL 1.005f

static void usage(const char *program)
{
    fprintf(stderr,
            "usage: %s [--size N] [--press KEYS] [--screenshot FILE] [--bench N]\n"
            "  --size N          window edge in pixels (default %d)\n"
            "  --press KEYS      feed these key presses before the first frame\n"
            "  --screenshot FILE render one frame into FILE as BMP, then exit\n"
            "  --bench N         render N frames, print the average time, then exit\n"
            "\n"
            "keys: 1 subdivision  2 textures  3 shadows  4 antialiasing  5 transparency\n"
            "      f fill lights  h help  w/a/s/d orbit  +/- zoom  r reset  c screenshot  q quit\n",
            program, window_size);
}

static int parse_args(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--screenshot") == 0 && i + 1 < argc) {
            screenshot_path = argv[++i];
        } else if (strcmp(argv[i], "--press") == 0 && i + 1 < argc) {
            scripted_keys = argv[++i];
        } else if (strcmp(argv[i], "--bench") == 0 && i + 1 < argc) {
            bench_frames = atoi(argv[++i]);
            if (bench_frames < 1) {
                fprintf(stderr, "--bench must be at least 1\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            window_size = atoi(argv[++i]);
            if (window_size < 64) {
                fprintf(stderr, "--size must be at least 64\n");
                return -1;
            }
        } else {
            usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

static int save_screenshot(const char *path)
{
    glFinish();

    if (screenshot_save_bmp(path, viewport_width, viewport_height) != 0) {
        fprintf(stderr, "cannot write %s\n", path);
        return -1;
    }

    printf("wrote %s (%dx%d)\n", path, viewport_width, viewport_height);
    return 0;
}

static void finish_benchmark(void)
{
    if (frames_rendered == SCREENSHOT_WARMUP_FRAMES) {
        bench_started_at = glutGet(GLUT_ELAPSED_TIME);
    }
    if (frames_rendered < SCREENSHOT_WARMUP_FRAMES + bench_frames) {
        glutPostRedisplay();
        return;
    }

    printf("%d frames at %dx%d: %.2f ms/frame\n", bench_frames, viewport_width, viewport_height,
           (double)(glutGet(GLUT_ELAPSED_TIME) - bench_started_at) / bench_frames);
    exit(EXIT_SUCCESS);
}

static void display(void)
{
    renderer_draw_frame(&app, viewport_width, viewport_height);
    frames_rendered++;

    if (bench_frames > 0) {
        glFinish();
        finish_benchmark();
        return;
    }

    if (screenshot_path != NULL) {
        if (frames_rendered >= SCREENSHOT_WARMUP_FRAMES) {
            exit(save_screenshot(screenshot_path) == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        }
        glutPostRedisplay();
    } else if (capture_requested) {
        char path[64];

        capture_requested = 0;
        snprintf(path, sizeof path, "cornellbox-%d.bmp", ++captures_taken);
        save_screenshot(path);
    }

    glutSwapBuffers();
}

static void reshape(int width, int height)
{
    viewport_width = width;
    viewport_height = height;
    glViewport(0, 0, width, height);
}

static int handle_camera_key(unsigned char key)
{
    switch (key) {
    case 'a':
        camera_orbit(-ORBIT_STEP_DEGREES, 0.0f);
        return 1;
    case 'd':
        camera_orbit(ORBIT_STEP_DEGREES, 0.0f);
        return 1;
    case 'w':
        camera_orbit(0.0f, ORBIT_STEP_DEGREES);
        return 1;
    case 's':
        camera_orbit(0.0f, -ORBIT_STEP_DEGREES);
        return 1;
    case '+':
    case '=':
        camera_zoom(1.0f / ZOOM_STEP);
        return 1;
    case '-':
        camera_zoom(ZOOM_STEP);
        return 1;
    case 'r':
        camera_reset();
        return 1;
    default:
        return 0;
    }
}

static int dispatch_key(unsigned char key)
{
    return handle_camera_key(key) || app_handle_key(&app, key);
}

static void handle_key(unsigned char key)
{
    if (dispatch_key(key)) {
        glutPostRedisplay();
    }
}

static void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    key = (unsigned char)tolower(key);
    switch (key) {
    case 27:
    case 'q':
        exit(EXIT_SUCCESS);
    case 'c':
        capture_requested = 1;
        glutPostRedisplay();
        return;
    default:
        handle_key(key);
    }
}

static void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN && (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)) {
        drag_button = button;
        drag_x = x;
        drag_y = y;
    } else if (state == GLUT_UP) {
        drag_button = -1;
    }
}

static void motion(int x, int y)
{
    const int dx = x - drag_x;
    const int dy = y - drag_y;

    drag_x = x;
    drag_y = y;

    if (drag_button == GLUT_LEFT_BUTTON) {
        camera_orbit((float)dx * ORBIT_DEGREES_PER_PIXEL, (float)dy * ORBIT_DEGREES_PER_PIXEL);
    } else if (drag_button == GLUT_RIGHT_BUTTON) {
        camera_zoom(powf(ZOOM_PER_PIXEL, (float)dy));
    } else {
        return;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    const char *key;

    glutInit(&argc, argv);

    if (parse_args(argc, argv) != 0) {
        return EXIT_FAILURE;
    }

    app_init(&app);
    camera_reset();
    for (key = scripted_keys; *key != '\0'; key++) {
        dispatch_key((unsigned char)*key);
    }

    glutInitDisplayString("rgb double depth samples=4");
    glutInitWindowSize(window_size, window_size);
    glutCreateWindow("Cornell Box");

    printf("OpenGL %s on %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    renderer_init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    return EXIT_SUCCESS;
}
