#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include "arena.h"
#include "raylib.h"

#define SCREEN_WIDTH 948
#define SCREEN_HEIGHT 533

typedef enum GameStates
{
    STATE_NULL,
    STATE_MENU_STATE,
    STATE_CUTSCENE_STATE,
    STATE_GAMEPLAY_STATE,
} GameStates;

typedef enum TextureId
{
    TEXTURE_NONE,
    TEXTURE_POPUP_BG,
    TEXTURE_INTERFACE_BG,
    TEXTURE_JDB,
    TEXTURE_BUG,
    TEXTURE_ALIENT_PORTRAIT_1,
    TEXTURE_ALIENT_PORTRAIT_2,
    TEXTURE_ALIENT_PORTRAIT_3,
    TEXTURE_ALIENT_PORTRAIT_4,
    TEXTURE_ALIENT_PORTRAIT_5,
    TEXTURE_ALIENT_PORTRAIT_6,
    TEXTURE_ALIENT_PORTRAIT_7,
    TEXTURE_ALIENT_PORTRAIT_8,
    TEXTURE_IB_1,
    TEXTURE_IB_2,
    TEXTURE_IB_3,
    TEXTURE_IB_4,
    TEXTURE_IB_5,
    TEXTURE_IB_6,
    TEXTURE_IB_7,
    TEXTURE_CUTSCENE_1,
    TEXTURE_CUTSCENE_2,
    TEXTURE_CUTSCENE_3,
    TEXTURE_CUTSCENE_4,
    TEXTURE_CUTSCENE_5,
    TEXTURE_CUTSCENE_6,
    TEXTURE_CUTSCENE_7,
    TEXTURE_CUTSCENE_8,
    TEXTURE_CUTSCENE_9,
    TEXTURE_CUTSCENE_11,
    TEXTURE_CUTSCENE_12,
    TEXTURE_CUTSCENE_15,
    TEXTURE_CUTSCENE_16,
    TEXTURE_CUTSCENE_17,
    TEXTURE_CUTSCENE_18,
    TEXTURE_CUTSCENE_19,
    TEXTURE_CUTSCENE_20,
    TEXTURE_CUTSCENE_21,
    TEXTURE_CUTSCENE_22,
    TEXTURE_CUTSCENE_23,
    TEXTURE_CUTSCENE_24,
    TEXTURE_CUTSCENE_25,
    TEXTURE_CUTSCENE_26,
    TEXTURE_CUTSCENE_27,
    TEXTURE_MAX,
} TextureId;

typedef struct GlobalContext {
    Arena frameArena;
    Arena stateArena;
    Arena globalArena;
    Texture2D textures[TEXTURE_MAX];
} GlobalContext;

extern GlobalContext g_context;

Texture2D GetRandomTexture(TextureId startId, TextureId endId);
#endif // GLOBAL_CONTEXT_H