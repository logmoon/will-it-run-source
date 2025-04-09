#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "arena.h"

typedef void (*state_function)(void);

typedef struct GameState
{
    state_function enter;
    state_function update;
    state_function draw;
    state_function exit;
} GameState;

void change_state(int newState);
void add_state(int index, state_function update, state_function draw, state_function enter, state_function exit);
void update_current_state(void);
void draw_current_state(void);

#endif // STATE_MANAGER_H