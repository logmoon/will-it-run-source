#include "state_manager.h"
#include "global_context.h"
#include <stddef.h>
#include <stdio.h>

#define MAX_STATES 10

static GameState states[MAX_STATES];
static int currentState = -1;

void change_state(int newState)
{
    if (newState >= 0 && newState < MAX_STATES)
    {
        if (currentState != -1)
        {
            if (states[currentState].exit != NULL)
            {
                states[currentState].exit();
            }
            arena_reset(&g_context.stateArena); // Reset state arena
        }
        if (states[newState].enter != NULL)
        {
            states[newState].enter();
        }
        currentState = newState;
    }
}

void add_state(int index, state_function update, state_function draw, state_function enter, state_function exit)
{
    if (index >= 0 && index < MAX_STATES)
    {
        states[index] = (GameState){ .update = update, .draw = draw, .enter = enter, .exit = exit };
    }
}

void update_current_state(void)
{
    if (currentState != -1 && states[currentState].update != NULL)
    {
        states[currentState].update();
    }
}

void draw_current_state(void)
{
    if (currentState != -1 && states[currentState].draw != NULL)
    {
        states[currentState].draw();
    }
}