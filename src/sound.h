#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include "arena.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct SoundContainer
{
    Vector2 source_position;           // Position where the sound originates
    int strength;                      // How loud the sound is
    bool active;                       // Active or not
    float duration;                      // How long the sound lasts (in seconds)
    float current_duration;              // Tracks how long the sound has been active
    size_t source_entity_index;        // Index of the entity that created the sound
} SoundContainer;
#define MAX_SOUNDS 100

typedef struct SoundInfo
{
    int strength;
    size_t source_entity;
} SoundInfo;

typedef struct SoundSystem
{
    SoundContainer* sounds;
    SoundInfo* sound_grid;
    size_t grid_width;
    size_t grid_height;
    Arena* arena;
} SoundSystem;

// Initializes the sound system
void sound_system_init(SoundSystem* sound_system, size_t level_grid_width, size_t level_grid_height, Arena* arena);
// Should be called at the beginning of each frame
void sound_system_reset(SoundSystem* sound_system);

// Updates the sound system
void sound_system_update(SoundSystem* sound_system, int* level_grid, int tile_size);

// Creates a new sound
void create_sound(SoundSystem* sound_system, Vector2 position, int strength, float duration, size_t source_entity_index);
// Returns true if a sound can be heard in a position.
bool can_hear_sound_at_position(SoundSystem* sound_system, Vector2 position, int tile_size, size_t* source_entity_index, int* strength);

// Visualize active sounds
void visualize_sounds(SoundSystem* sound_system, int tile_size);

#endif // SOUND_H