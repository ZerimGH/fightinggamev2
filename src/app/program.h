#ifndef PROGRAM_H

#define PROGRAM_H

typedef enum {
    PS_MENU,        /* Main menu */
    PS_LAN_MENU,    /* LAN menu */
    PS_WAITING,     /* Waiting for a LAN game to start */
    PS_SERVER_MENU, /* Display a list of LAN servers to join */
    PS_HOST,        /* Get server options before hosting */
    PS_GAMING,      /* In a game */
} ProgramState;

int program_init(void);
void program_update(void);
void program_render(void);
int program_state_change(ProgramState new_state);
void program_deinit(void);
ProgramState program_get_state(void);

#endif /* PROGRAM_H */
