#include "program.h"
#include "client.h"
#include "log.h"
#include "program_gaming.h"
#include "program_host.h"
#include "program_lan_menu.h"
#include "program_menu.h"
#include "program_server_menu.h"
#include "program_waiting.h"
#include "raylib/raylib.h"
#include "server.h"
#include <stdlib.h>

#define DO_MAGIC                                                                 \
    X(PROGRAM_STATE_MENU, program_menu)                                                     \
    X(PROGRAM_STATE_LAN_MENU, program_lan_menu)                                             \
    X(PROGRAM_STATE_GAMING, program_gaming)                                                 \
    X(PROGRAM_STATE_WAITING, program_waiting)                                               \
    X(PROGRAM_STATE_SERVER_MENU, program_server_menu)                                       \
    X(PROGRAM_STATE_HOST, program_host)

static ProgramState state = -1;

static int state_enter(ProgramState new_state);

int program_init(void) {
    if (state != (ProgramState)-1) { return 1; }
    state_enter(PROGRAM_STATE_MENU);
    return 0;
}

static int state_enter(ProgramState new_state) {
    if (new_state != PROGRAM_STATE_GAMING) SetTargetFPS(30);
    else SetTargetFPS(120); /* Should be fine ? */
    switch (new_state) {
#define X(STATE, PREFIX)                                                         \
    case (STATE):                                                                \
        if (PREFIX##_enter()) return 1;                                          \
        state = new_state;                                                       \
        return 0;
        DO_MAGIC
#undef X
    default: PERROR("Unhandled state %d\n", state); return 1;
    }
    return 1;
}

static void state_exit(void) {
    switch (state) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_exit(); break;
        DO_MAGIC
#undef X
    default: PERROR("Unhandled state %d\n", state); return;
    }
}

int program_state_change(ProgramState new_state) {
    if (new_state == state) { return 0; }
    state_exit();
    if (state_enter(new_state)) {
        PERROR("Failed to enter state %d, going to menu.\n", new_state);
        state_enter(PROGRAM_STATE_MENU);
        return 1;
    }
    return 0;
}

void program_update(void) {
    switch (state) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_update(); break;
        DO_MAGIC
#undef X
    default: PERROR("Unhandled state %d\n", state); return;
    }
}

void program_render(void) {
    switch (state) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_render(); break;
        DO_MAGIC
#undef X
    default: PERROR("Unhandled state %d\n", state); return;
    }
}

void program_deinit(void) {
    state_exit();
    state = (ProgramState)-1;
    if (client_is_init()) client_deinit();
    if (server_is_init()) server_deinit();
}

ProgramState program_get_state(void) { return state; }

#undef DO_MAGIC
