#ifndef APP_H
#define APP_H

typedef struct {
    int subdivision_steps;
    int textures;
    int shadows;
    int antialiasing;
    int transparency;
    int fill_lights;
    int help;
} AppState;

void app_init(AppState *state);

int app_handle_key(AppState *state, unsigned char key);

#endif
