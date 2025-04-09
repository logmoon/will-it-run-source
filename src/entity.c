#include "entity.h"
#include "raymath.h"
#include "gameplay_state.h"
#include "ide.h"
#include <float.h>
#include <stdlib.h>

static Entity entities[MAX_ENTITIES];
Entity* player;

void init_entities(void)
{
    player = NULL; // Reset player reference
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        entities[i].active = false;
    }
}

Entity* create_entity(EntityType type, Vector2 position, TextureId textureId)
{
    size_t index = MAX_ENTITIES + 1;
    for (size_t i = 0; i < MAX_ENTITIES; i++)
    {
        if (!entities[i].active)
        {
            index = i;
            break;
        }
    }
    if (index == MAX_ENTITIES + 1) return NULL;

    Entity* entity = &entities[index];
    entity->type = type;
    entity->texture_id = textureId;
    entity->position = position;
    entity->velocity = (Vector2){0, 0};
    entity->active = true;
    entity->stay_timer = 0.0f;
    entity->behavior_type = PLAYER_BEHAVIOR_ROAM_AND_CHASE;
    entity->facing_direction = 1;

    if (type == ENTITY_JDB)
    {
        player = entity; // Assign the player reference
        entity->speed = 50.0f; // Set player speed here
        entity->attack_speed = 0.5f;
    }
    else if (type == ENTITY_BUG)
    {
        entity->speed = 25.0f; // Set enemy speed here
    }

    return entity;
}

int fsign(float a)
{
    return a >= 0.0f ? 1 : -1;
}
void update_entities(float delta_time, Rectangle bounds, Editor* editor)
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        Entity* entity = &entities[i];
        if (!entity->active) continue;

        entity->attack_timer += delta_time;

        if (entity->type == ENTITY_BUG)
        {
            // Enemies chase the player with random deviations
            Vector2 direction = Vector2Subtract(player->position, entity->position);
            direction = Vector2Normalize(direction);

            // Add random deviation to the direction
            direction.x += GetRandomValue(-10, 10) * 0.01f;
            direction.y += GetRandomValue(-10, 10) * 0.01f;

            float distanceFromPlayer = Vector2Distance(entity->position, player->position);
            if (distanceFromPlayer > 30)
            {
                entity->velocity = Vector2Scale(direction, entity->speed);
            }
            if (player->behavior_type == PLAYER_BEHAVIOR_STAY_IN_PLACE && distanceFromPlayer <= 40)
            {
                // Hit Player
                PlaySound(gameplay_data.build_error_sfx);
                gameplay_data.build = false;
                player->behavior_type = PLAYER_BEHAVIOR_ROAM_AND_CHASE;
                player->stay_timer = 0.0f;
            }
        }
        else if (entity->type == ENTITY_JDB)
        {
            if (entity->velocity.x != 0.0f)
            {
                if (fsign(entity->velocity.x) != entity->facing_direction)
                {
                    entity->facing_direction *= -1;
                }
            }

            if (entity->behavior_type == PLAYER_BEHAVIOR_ROAM_AND_CHASE)
            {
                entity->speed = 50.0f; // Set player speed here

                // Player roams and chases the nearest enemy
                Entity* nearest_enemy = NULL;
                float nearest_dist = FLT_MAX;
                
                for (int j = 0; j < MAX_ENTITIES; j++)
                {
                    if (!entities[j].active) continue;

                    if (entities[j].type == ENTITY_BUG && entities[j].active)
                    {
                        float dist = Vector2Distance(entities[j].position, entity->position);
                        if (dist <= 40)
                        {
                            if (entity->attack_timer >= entity->attack_speed)
                            {
                                remove_entity(&entities[j]);
                                entity->attack_timer = 0.0f;
                                continue;
                            }
                        }

                        if (dist < nearest_dist)
                        {
                            nearest_dist = dist;
                            nearest_enemy = &entities[j];
                        }
                    }
                }

                if (nearest_enemy)
                {
                    Vector2 direction = Vector2Subtract(nearest_enemy->position, entity->position);
                    entity->velocity = Vector2Scale(Vector2Normalize(direction), entity->speed);
                }
                else
                {
                    entity->velocity = Vector2Zero();
                }
            }
            else if (entity->behavior_type == PLAYER_BEHAVIOR_STAY_IN_PLACE)
            {
                entity->speed = 100.0f; // Set player speed here
                // Player moves to target position and stays there
                Vector2 direction = Vector2Subtract(entity->target_position, entity->position);
                if (Vector2Length(direction) > 1.0f)
                {
                    entity->velocity = Vector2Scale(Vector2Normalize(direction), entity->speed);
                }
                else
                {
                    entity->velocity = (Vector2){0, 0};
                    entity->stay_timer += delta_time;
                    if (entity->stay_timer >= 2.0f)
                    {
                        build_commit(editor);
                        entity->behavior_type = PLAYER_BEHAVIOR_ROAM_AND_CHASE;  // Switch back to roaming
                        entity->stay_timer = 0.0f;
                    }
                }
            }
        }

        entity->position.x += entity->velocity.x * delta_time;
        entity->position.y += entity->velocity.y * delta_time;

        // Keep entity within bounds
        if (entity->position.x < bounds.x) entity->position.x = bounds.x;
        if (entity->position.y < bounds.y) entity->position.y = bounds.y;
        if (entity->position.x > bounds.x + bounds.width) entity->position.x = bounds.x + bounds.width;
        if (entity->position.y > bounds.y + bounds.height) entity->position.y = bounds.y + bounds.height;
    }
}

void draw_entities(void)
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (!entities[i].active) continue;
        DrawTexture(g_context.textures[entities[i].texture_id], entities[i].position.x, entities[i].position.y, WHITE);
    }
}

void remove_entity(Entity* entity)
{
    if (entity)
    {
        entity->active = false;
    }
}

void clear_entities(void)
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        entities[i].active = false;
    }
}