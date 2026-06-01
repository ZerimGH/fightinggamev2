#ifndef PROGRAM_H

#define PROGRAM_H

typedef enum {
    PS_MENU,
    PS_LAN_MENU,
    PS_GAMING,
} ProgramState;

int program_init(void);
void program_update(void);
void program_render(void);
int program_change_state(ProgramState new_state);
void program_deinit(void);
ProgramState program_get_state(void);

#endif /* PROGRAM_H */
