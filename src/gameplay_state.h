// gameplay_state.h
#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include <stdbool.h>
#include "entity.h"

typedef struct StateData
{
    bool docs_popup;
    bool job_popup;
    float enemy_spawn_timer;
    bool build;
    float game_timer;
    Texture2D current_alien_texture;

    Sound build_success_sfx;
    Sound build_error_sfx;
    Sound type_sfx;
    Sound job_popup_sfx;
} StateData;

extern StateData gameplay_data;

void gameplay_state_enter(void);
void gameplay_state_update(void);
void gameplay_state_draw(void);
void gameplay_state_exit(void);

#endif // GAMEPLAY_STATE_H