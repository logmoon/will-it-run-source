#include "gameplay_state.h"
#include "ide.h"
#include "global_context.h"
#include "script.h"
#include "game_app.h"
#include "popup.h"
#include <stdio.h>
#include <string.h>
#include <raymath.h>
#include "state_manager.h"

StateData gameplay_data;

// Define the proportion of screen split (0.5 = split in half)
#define IDE_SCREEN_RATIO 0.5

Editor* editor;

// Define IDE screen rectangle
Rectangle ideScreen = { 0, 0, SCREEN_WIDTH * IDE_SCREEN_RATIO, SCREEN_HEIGHT };

// Define game screen rectangle
Rectangle gameScreen = { SCREEN_WIDTH * IDE_SCREEN_RATIO, 0, SCREEN_WIDTH * (1.0f - IDE_SCREEN_RATIO), SCREEN_HEIGHT };

Structure structure = {0};
Script script = {0};

Sound alien_talk;

#define ENEMY_SPAWN_INTERVAL 1.0f
#define GAME_TIME 300.0f

void gameplay_state_enter(void)
{
    gameplay_data.game_timer = 0.0f;

    alien_talk = LoadSound("resources/alien_talk.wav");
    PlaySound(alien_talk);

    gameplay_data.build_success_sfx = LoadSound("resources/build_success.wav");
    gameplay_data.build_error_sfx = LoadSound("resources/build_success.wav");
    gameplay_data.type_sfx = LoadSound("resources/type.wav");
    gameplay_data.job_popup_sfx = LoadSound("resources/job_popup.wav");

    editor = ide_init();
    // Generate a structure, a script and the description of the script
    structure = generate_structure(1, 3, 3, 5);
    script = structure_to_script(&structure);
    generate_description(&script);

    gameplay_data.current_alien_texture = GetRandomTexture(TEXTURE_ALIENT_PORTRAIT_1, TEXTURE_ALIENT_PORTRAIT_8);

    // Initialize entity system
    init_entities();

    // Create player
    Vector2 player_pos = {gameScreen.x + gameScreen.width / 2, gameScreen.y + gameScreen.height / 2};
    create_entity(ENTITY_JDB, player_pos, TEXTURE_JDB);

    // Initialize enemy spawn timer
    gameplay_data.enemy_spawn_timer = 0.0f;

    // Show job
    gameplay_data.job_popup = true;
    PlaySound(gameplay_data.job_popup_sfx);
}

void gameplay_state_update(void)
{
    float delta_time = GetFrameTime();

    if (editor->currentCodeBlock >= structure.block_count + 1 && editor->codeBlocks[editor->currentCodeBlock - 1].built)
    {
        // Win
        change_state(STATE_MENU_STATE);
        return;
    }

    gameplay_data.game_timer += delta_time;

    if (gameplay_data.game_timer >= GAME_TIME)
    {
        // Lose
        change_state(STATE_MENU_STATE);
        return;
    }

    // Handle player behaviors
    if (gameplay_data.build)
    {
        // Switch player to "stay in place" behavior
        player->behavior_type = PLAYER_BEHAVIOR_STAY_IN_PLACE;
        float x = gameScreen.x + GetRandomValue(0, (int)gameScreen.width);
        float y = gameScreen.y + GetRandomValue(0, (int)gameScreen.height);
        if (editor->currentCodeBlock == 0)
        {
            x = gameScreen.x;
            y = gameScreen.y;
        }
        else
        {
            Block* b = &structure.blocks[editor->currentCodeBlock - 1];
            int block_size_x = gameScreen.width / structure.canvas.grid_size[0];
            int block_size_y = (gameScreen.height - 40) / structure.canvas.grid_size[1];
            x = gameScreen.x + (b->grid_pos[0] * block_size_x);
            y = gameScreen.y + 40 + (b->grid_pos[1] * block_size_y);
        }

        player->target_position = (Vector2)
        {
            x,y
        };
    }

    // Spawn enemies
    gameplay_data.enemy_spawn_timer += delta_time;
    if (gameplay_data.enemy_spawn_timer >= ENEMY_SPAWN_INTERVAL)
    {
        gameplay_data.enemy_spawn_timer = 0.0f;
        Vector2 enemy_pos = {
            gameScreen.x + GetRandomValue(0, (int)gameScreen.width),
            gameScreen.y + GetRandomValue(0, (int)gameScreen.height)
        };
        create_entity(ENTITY_BUG, enemy_pos, TEXTURE_BUG);
    }

    // Update all entities, passing the player to the update function
    update_entities(delta_time, gameScreen, editor);

    if (!gameplay_data.docs_popup && !gameplay_data.job_popup)
    {
        ide_update(editor, ideScreen, script);
    }

    if (IsKeyPressed(KEY_TAB))
    {
        if (gameplay_data.docs_popup)
        {
            PlaySound(gameplay_data.job_popup_sfx);
            gameplay_data.docs_popup = false;
        }
        else if (gameplay_data.job_popup)
        {
            PlaySound(gameplay_data.job_popup_sfx);
            gameplay_data.job_popup = false;
        }
    }
}

void gameplay_state_draw(void)
{
    // Draw IDE screen (left side)
    DrawRectangleRec(ideScreen, LIGHTGRAY);
    ide_render(editor, ideScreen);

    // Draw Game/Application screen (right side)
    DrawRectangleRec(gameScreen, GRAY);
    game_app_render(gameScreen, *editor, structure);

    // Draw entities
    draw_entities();

    // Draw timer
    char timerText[32];
    snprintf(timerText, sizeof(timerText), "Job Time Limit: %.0f", GAME_TIME - gameplay_data.game_timer);
    int timerFontSize = 20;
    Vector2 timerSize = MeasureTextEx(GetFontDefault(), timerText, timerFontSize, 1);
    DrawText(timerText, 
             (GetScreenWidth() - timerSize.x) / 2, 
             GetScreenHeight() - 30, 
             timerFontSize, 
             WHITE);


    // Draw popups if available
    if (gameplay_data.docs_popup)
    {
        popup_render_docs();
    }
    else if (gameplay_data.job_popup)
    {
        popup_render_job(*editor, script);
    }
}

void gameplay_state_exit(void)
{
    UnloadSound(alien_talk);
    UnloadSound(gameplay_data.build_error_sfx);
    UnloadSound(gameplay_data.build_success_sfx);
    UnloadSound(gameplay_data.job_popup_sfx);
    UnloadSound(gameplay_data.type_sfx);
    clear_entities();
    free_script(structure, script);
}