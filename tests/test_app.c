#include "app.h"
#include "check.h"

static void test_defaults(void)
{
    AppState state;

    app_init(&state);

    check(state.subdivision_steps == 32, "default subdivision is 32");
    check(state.textures == 1, "textures on by default");
    check(state.shadows == 1, "shadows on by default");
    check(state.transparency == 1, "transparency on by default");
    check(state.antialiasing == 1, "antialiasing on by default");
    check(state.fill_lights == 1, "fill lights on by default");
    check(state.help == 1, "help shown by default");
}

static void test_subdivision_cycles(void)
{
    AppState state;

    app_init(&state);

    check(app_handle_key(&state, '1') && state.subdivision_steps == 1, "32 wraps to 1");
    check(app_handle_key(&state, '1') && state.subdivision_steps == 8, "1 steps to 8");
    check(app_handle_key(&state, '1') && state.subdivision_steps == 32, "8 steps to 32");
}

static void test_toggles(void)
{
    AppState state;

    app_init(&state);

    check(app_handle_key(&state, '4') && state.antialiasing == 0, "4 turns antialiasing off");
    check(app_handle_key(&state, '4') && state.antialiasing == 1, "4 turns antialiasing back on");
    check(app_handle_key(&state, 'f') && state.fill_lights == 0, "f turns fill lights off");
    check(app_handle_key(&state, '3') && state.shadows == 0, "3 turns shadows off");
    check(app_handle_key(&state, '2') && state.textures == 0, "2 turns textures off");
    check(app_handle_key(&state, '5') && state.transparency == 0, "5 turns transparency off");
    check(app_handle_key(&state, 'h') && state.help == 0, "h hides help");
}

static void test_unknown_key_is_ignored(void)
{
    AppState state;
    AppState before;

    app_init(&state);
    before = state;

    check(app_handle_key(&state, 'z') == 0, "unknown key is ignored");
    check(state.subdivision_steps == before.subdivision_steps &&
          state.textures == before.textures &&
          state.transparency == before.transparency &&
          state.shadows == before.shadows &&
          state.antialiasing == before.antialiasing &&
          state.fill_lights == before.fill_lights &&
          state.help == before.help, "unknown key: state unchanged");
}

void test_app_main(void)
{
    test_defaults();
    test_subdivision_cycles();
    test_toggles();
    test_unknown_key_is_ignored();
}
