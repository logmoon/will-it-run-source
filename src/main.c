#include "raylib.h"
#include "raymath.h"
#include "cLDtk.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pathfinding.h"
#include "global_context.h"
#include "state_manager.h"
#include "gameplay_state.h"
#include "cutscene_state.h"
#include "menu_state.h"
#define ARENA_IMPLEMENTATION
#include "arena.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static void frame(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Will It Run?");
    SetExitKey(KEY_NULL); // Removes espace key closing the game

    InitAudioDevice();      // Initialize audio device
    SetWindowIcon(LoadImage("resources/logo.png"));

    Music music = LoadMusicStream("resources/music.mp3");
    PlayMusicStream(music);

    // Initializing arenas
    g_context.globalArena = (Arena)ARENA_INIT;
    g_context.stateArena = (Arena)ARENA_INIT;
    g_context.frameArena = (Arena)ARENA_INIT;

    // Loading textures
    g_context.textures[TEXTURE_NONE] = LoadTexture("resources/missing_texture.png");
    g_context.textures[TEXTURE_POPUP_BG] = LoadTexture("resources/interface5.png");
    g_context.textures[TEXTURE_INTERFACE_BG] = LoadTexture("resources/interface_bg.png");
    g_context.textures[TEXTURE_JDB] = LoadTexture("resources/jdb.png");
    g_context.textures[TEXTURE_BUG] = LoadTexture("resources/bug.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_1] = LoadTexture("resources/avatar1.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_2] = LoadTexture("resources/avatar2.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_3] = LoadTexture("resources/avatar3.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_4] = LoadTexture("resources/avatar4.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_5] = LoadTexture("resources/avatar5.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_6] = LoadTexture("resources/avatar6.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_7] = LoadTexture("resources/avatar7.png");
    g_context.textures[TEXTURE_ALIENT_PORTRAIT_8] = LoadTexture("resources/avatar8.png");
    g_context.textures[TEXTURE_IB_1] = LoadTexture("resources/ib1.png");
    g_context.textures[TEXTURE_IB_2] = LoadTexture("resources/ib2.png");
    g_context.textures[TEXTURE_IB_3] = LoadTexture("resources/ib3.png");
    g_context.textures[TEXTURE_IB_4] = LoadTexture("resources/ib4.png");
    g_context.textures[TEXTURE_IB_5] = LoadTexture("resources/ib5.png");
    g_context.textures[TEXTURE_IB_6] = LoadTexture("resources/ib6.png");
    g_context.textures[TEXTURE_IB_7] = LoadTexture("resources/ib7.png");
    g_context.textures[TEXTURE_CUTSCENE_1] = LoadTexture("resources/animation0.png");
    g_context.textures[TEXTURE_CUTSCENE_2] = LoadTexture("resources/animation1.png");
    g_context.textures[TEXTURE_CUTSCENE_3] = LoadTexture("resources/animation2.png");
    g_context.textures[TEXTURE_CUTSCENE_4] = LoadTexture("resources/animation3.png");
    g_context.textures[TEXTURE_CUTSCENE_5] = LoadTexture("resources/animation4.png");
    g_context.textures[TEXTURE_CUTSCENE_6] = LoadTexture("resources/animation5.png");
    g_context.textures[TEXTURE_CUTSCENE_7] = LoadTexture("resources/animation6.png");
    g_context.textures[TEXTURE_CUTSCENE_8] = LoadTexture("resources/animation7.png");
    g_context.textures[TEXTURE_CUTSCENE_9] = LoadTexture("resources/animation8.png");
    g_context.textures[TEXTURE_CUTSCENE_11] = LoadTexture("resources/animation9.png");
    g_context.textures[TEXTURE_CUTSCENE_12] = LoadTexture("resources/animation11.png");
    g_context.textures[TEXTURE_CUTSCENE_15] = LoadTexture("resources/animation12.png");
    g_context.textures[TEXTURE_CUTSCENE_16] = LoadTexture("resources/animation15.png");
    g_context.textures[TEXTURE_CUTSCENE_17] = LoadTexture("resources/animation16.png");
    g_context.textures[TEXTURE_CUTSCENE_18] = LoadTexture("resources/animation17.png");
    g_context.textures[TEXTURE_CUTSCENE_19] = LoadTexture("resources/animation18.png");
    g_context.textures[TEXTURE_CUTSCENE_20] = LoadTexture("resources/animation19.png");
    g_context.textures[TEXTURE_CUTSCENE_21] = LoadTexture("resources/animation20.png");
    g_context.textures[TEXTURE_CUTSCENE_22] = LoadTexture("resources/animation21.png");
    g_context.textures[TEXTURE_CUTSCENE_23] = LoadTexture("resources/animation22.png");
    g_context.textures[TEXTURE_CUTSCENE_24] = LoadTexture("resources/animation23.png");
    g_context.textures[TEXTURE_CUTSCENE_25] = LoadTexture("resources/animation24.png");
    g_context.textures[TEXTURE_CUTSCENE_26] = LoadTexture("resources/animation25.png");
    g_context.textures[TEXTURE_CUTSCENE_27] = LoadTexture("resources/animation26.png");

    // Adding states
    add_state(STATE_NULL, NULL, NULL, NULL, NULL);
    add_state(STATE_GAMEPLAY_STATE, &gameplay_state_update, &gameplay_state_draw, &gameplay_state_enter, &gameplay_state_exit);
    add_state(STATE_MENU_STATE, &menu_state_update, &menu_state_draw, &menu_state_enter, &menu_state_exit);
    add_state(STATE_CUTSCENE_STATE, &cutscene_state_update, &cutscene_state_draw, &cutscene_state_enter, &cutscene_state_exit);
    change_state(STATE_MENU_STATE);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(frame, 60, 1);
#else

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateMusicStream(music);
        frame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    arena_deinit(&g_context.frameArena);
    arena_deinit(&g_context.stateArena);
    arena_deinit(&g_context.globalArena);
    UnloadMusicStream(music);
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void frame(void)
{
    arena_reset(&g_context.frameArena);

    // Update
    //----------------------------------------------------------------------------------
    update_current_state();
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    {    
        ClearBackground(BLACK);
        draw_current_state();
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
}