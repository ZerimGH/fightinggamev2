#ifndef PROGRAM_H

#define PROGRAM_H

typedef enum {
    PROGRAM_STATE_MENU,        /* Main menu */
    PROGRAM_STATE_LAN_MENU,    /* LAN menu */
    PROGRAM_STATE_WAITING,     /* Waiting for a LAN game to start */
    PROGRAM_STATE_SERVER_MENU, /* Display a list of LAN servers to join */
    PROGRAM_STATE_HOST,        /* Get server options before hosting */
    PROGRAM_STATE_GAMING,      /* In a game */
} ProgramState;

int program_init(void);
void program_update(void);
void program_render(void);
int program_state_change(ProgramState new_state);
void program_deinit(void);
ProgramState program_get_state(void);

#endif /* PROGRAM_H */
