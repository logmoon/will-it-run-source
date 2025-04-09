#include "sound.h"
#include "utils.h"

#include <stdio.h>


// Initializes the sound system
void sound_system_init(SoundSystem* sound_system, size_t level_grid_width, size_t level_grid_height, Arena* arena)
{
    sound_system->arena = arena;
    sound_system->sounds = (SoundContainer*)arena_alloc(arena, MAX_SOUNDS * sizeof(SoundContainer));
    for (size_t i = 0; i < MAX_SOUNDS; ++i)
    {
        sound_system->sounds[i].active = false;
    }

    sound_system->grid_width = level_grid_width;
    sound_system->grid_height = level_grid_height;
    sound_system->sound_grid = (SoundInfo*)arena_alloc(arena, level_grid_width*level_grid_height*sizeof(SoundInfo));
}
// Should be called at the beginning of each frame
void sound_system_reset(SoundSystem* sound_system)
{
    int grid_size = sound_system->grid_width * sound_system->grid_height;
    for (int i = 0; i < grid_size; ++i)
    {
        sound_system->sound_grid[i].strength = 0;
        sound_system->sound_grid[i].source_entity = -1;
    }
}

// Propagates a sound
void propagate_sound(SoundContainer* sound, SoundInfo* sound_grid, int* level_grid, int grid_width, int grid_height, int tile_size)
{
    if (!sound->active) return;

    if (sound->current_duration >= sound->duration)
    {
        sound->active = false;
        return;
    }

    Node source_grid = world_to_grid(sound->source_position, tile_size);

    for (int y = 0; y < grid_height; y++) 
    {
        for (int x = 0; x < grid_width; x++)
        {
            int dx = abs(x - source_grid.x);
            int dy = abs(y - source_grid.y);
            int distance = dx + dy;

            // Determine the attenuation based on obstacles
            int attenuation = 0;
            int cx = source_grid.x;
            int cy = source_grid.y;
            int sx = (x > cx) ? 1 : (x < cx) ? -1 : 0;
            int sy = (y > cy) ? 1 : (y < cy) ? -1 : 0;

            while (cx != x || cy != y)
            {
                // Check if the current tile is blocked
                if (level_grid[cy * grid_width + cx] == 1)
                {
                    attenuation += 3;  // Increase attenuation for wall tiles
                }
                else if (level_grid[cy * grid_width + cx] == 2)
                {
                    attenuation += 2; // For doors
                }

                // Move to the next tile in the direction of the target
                if (cx != x) cx += sx;
                if (cy != y) cy += sy;
            }

            // Calculate the final strength considering distance and attenuation
            int strength_at_point = sound->strength - distance - attenuation;

            // Update the sound grid if this sound is audible at this point
            if (strength_at_point > 0)
            {
                int index = y * grid_width + x;
                if (sound_grid[index].strength < strength_at_point)
                {
                    sound_grid[index].strength = strength_at_point;
                    sound_grid[index].source_entity = sound->source_entity_index;
                }
            }
        }
    }

    sound->current_duration += GetFrameTime();
}
// Updates the sound system
void sound_system_update(SoundSystem* sound_system, int* level_grid, int tile_size)
{
    for (size_t i = 0; i < MAX_SOUNDS; i++)
    {
        propagate_sound(&sound_system->sounds[i], sound_system->sound_grid, level_grid, sound_system->grid_width, sound_system->grid_height, tile_size);
    }
}

// Creates a new sound
void create_sound(SoundSystem* sound_system, Vector2 position, int strength, float duration, size_t source_entity_index)
{
    for (size_t i = 0; i < MAX_SOUNDS; i++)
    {
        SoundContainer* sound = &sound_system->sounds[i];
        if (!sound->active)
        {
            sound_system->sounds[i] = CLITERAL(SoundContainer){
                .source_position = position,
                .strength = strength,
                .active = true,
                .duration = duration,
                .current_duration = 0.0f,
                .source_entity_index = source_entity_index
            };

            return;
        }
    }

    printf("Couldn't find any inactive sounds this frame\n");
}
// Returns true if a sound can be heard in a position.
bool can_hear_sound_at_position(SoundSystem* sound_system, Vector2 position, int tile_size, size_t* source_entity_index, int* strength)
{
    Node grid_pos = world_to_grid(position, tile_size);
    int index = grid_pos.y * sound_system->grid_width + grid_pos.x;

    // Check if there's any sound at this tile
    if (sound_system->sound_grid[index].strength > 0)
    {
        *source_entity_index = sound_system->sound_grid[index].source_entity;
        *strength = sound_system->sound_grid[index].strength;
        return true;
    }

    return false;
}

// Visualize active sounds
void visualize_sounds(SoundSystem* sound_system, int tile_size)
{
    // Draw the sound propagation as circles and highlights on the grid
    for (size_t i = 0; i < MAX_SOUNDS; i++)
    {
        SoundContainer* sound = &sound_system->sounds[i];
        if (sound->active)
        {
            // Draw a circle at the sound source position with a radius based on the sound's strength
            float radius = (float)(sound->strength * tile_size);
            DrawCircleV(sound->source_position, radius, Fade(RED, 0.3f));

            // Draw the sound propagation grid
            for (size_t y = 0; y < sound_system->grid_height; y++)
            {
                for (size_t x = 0; x < sound_system->grid_width; x++)
                {
                    int index = y * sound_system->grid_width + x;
                    if (sound_system->sound_grid[index].strength > 0)
                    {
                        // Draw a semi-transparent square to visualize the sound's reach
                        Vector2 tile_position = {
                            (float)(x * tile_size),
                            (float)(y * tile_size)
                        };
                        Color color = Fade(BLUE, (float)sound_system->sound_grid[index].strength / sound->strength);
                        DrawRectangleV(tile_position, (Vector2){tile_size, tile_size}, color);
                    }
                }
            }
        }
    }
}