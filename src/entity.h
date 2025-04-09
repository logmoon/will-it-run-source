#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "global_context.h"
#include "ide.h"
#include <stdbool.h>

#define MAX_ENTITIES 200

typedef enum {
    ENTITY_JDB,
    ENTITY_BUG
} EntityType;

typedef enum PlayerBehavior
{
    PLAYER_BEHAVIOR_ROAM_AND_CHASE,
    PLAYER_BEHAVIOR_STAY_IN_PLACE
} PlayerBehavior;

typedef struct Entity
{
    EntityType type;
    Vector2 position;
    Vector2 velocity;
    TextureId texture_id;;
    float speed;
    bool active;
    Vector2 target_position;
    PlayerBehavior behavior_type;
    float stay_timer;
    float attack_timer;
    float attack_speed;
    int facing_direction;
} Entity;

extern Entity* player;

void init_entities(void);
Entity* create_entity(EntityType type, Vector2 position, TextureId textureId);
void update_entities(float delta_time, Rectangle bounds, Editor* editor);
void draw_entities(void);
void remove_entity(Entity* entity);
void clear_entities(void);

#endif // ENTITY_H