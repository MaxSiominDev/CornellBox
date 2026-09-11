#include "app.h"

static const int subdivision_levels[] = {1, 8, 32};
#define SUBDIVISION_LEVEL_COUNT ((int)(sizeof subdivision_levels / sizeof subdivision_levels[0]))

static int next_subdivision(int current)
{
    int i;

    for (i = 0; i < SUBDIVISION_LEVEL_COUNT; i++) {
        if (subdivision_levels[i] == current) {
            return subdivision_levels[(i + 1) % SUBDIVISION_LEVEL_COUNT];
        }
    }

    return subdivision_levels[0];
}

void app_init(AppState *state)
{
    state->subdivision_steps = subdivision_levels[SUBDIVISION_LEVEL_COUNT - 1];
    state->textures = 1;
    state->shadows = 1;
    state->antialiasing = 1;
    state->transparency = 1;
    state->fill_lights = 1;
    state->help = 1;
}

int app_handle_key(AppState *state, unsigned char key)
{
    switch (key) {
    case '1':
        state->subdivision_steps = next_subdivision(state->subdivision_steps);
        return 1;
    case '2':
        state->textures = !state->textures;
        return 1;
    case '3':
        state->shadows = !state->shadows;
        return 1;
    case '4':
        state->antialiasing = !state->antialiasing;
        return 1;
    case '5':
        state->transparency = !state->transparency;
        return 1;
    case 'f':
        state->fill_lights = !state->fill_lights;
        return 1;
    case 'h':
        state->help = !state->help;
        return 1;
    default:
        return 0;
    }
}
