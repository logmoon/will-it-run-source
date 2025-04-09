#include "cutscene_state.h"
#include "state_manager.h"
#include "raylib.h"
#include "global_context.h"

float timer;
int currentIndex;
float delay;
int count;

void cutscene_state_enter(void)
{
    timer = 0.0f;
    delay = 0.15f;
    count = (int)TEXTURE_CUTSCENE_27;
    currentIndex = (int)TEXTURE_CUTSCENE_1;
}
void cutscene_state_update(void)
{
}
void cutscene_state_draw(void)
{
    timer += GetFrameTime();

    if (timer >= delay) {
        timer = 0;
        currentIndex++;

        if (currentIndex >= count)
        {
            change_state(STATE_GAMEPLAY_STATE);
        }
    }

    Texture2D currentTexture = g_context.textures[currentIndex];
    DrawTexturePro(currentTexture,
                   (Rectangle){ 0, 0, currentTexture.width, currentTexture.height },
                   (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() },
                   (Vector2){ 0, 0 }, 0, WHITE);
}
void cutscene_state_exit(void)
{
}