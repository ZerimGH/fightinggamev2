#ifndef GAME_H
#define GAME_H

/* Initialise the game */
int game_init(void);
/* Deinitialise the game */
void game_deinit(void);
/* Update the game, should be called once per frame at exactly 60fps */
void game_tick(void);
/* Render the game */
void game_render(void);

#endif /* GAME_H */
