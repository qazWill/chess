/* ensures GAME_H is only defined once */
#ifndef GAME_H
#define GAME_H

/* global variables */
extern int user_is_white;

/* functions */
void move_user(void);
void move_opponent(void);

#endif