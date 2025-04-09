#include "raylib.h"
#include "menu_state.h"
#include "state_manager.h"
#include "global_context.h"


void menu_state_enter(void)
{
}
void menu_state_update(void)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        change_state(STATE_CUTSCENE_STATE);
    }
}
void menu_state_draw(void)
{
    // Draw background
    Texture2D backgroundTexture = g_context.textures[TEXTURE_CUTSCENE_1];
    DrawTexturePro(backgroundTexture, 
                   (Rectangle){ 0, 0, backgroundTexture.width, backgroundTexture.height },
                   (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() },
                   (Vector2){ 0, 0 }, 0, WHITE);

    // Draw game title
    const char* title = "Will it run?";
    int titleFontSize = 60;
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, titleFontSize, 2);
    DrawText(title, (GetScreenWidth() - titleSize.x) / 2, 50, titleFontSize, RED);

    // Draw "Press Enter to start" text
    const char* startText = "Press Enter to start";
    int startFontSize = 30;
    Vector2 startSize = MeasureTextEx(GetFontDefault(), startText, startFontSize, 1);
    DrawText(startText, (GetScreenWidth() - startSize.x) / 2, GetScreenHeight() - 80, startFontSize, RED);

    // Draw tutorial text
    const char* tutorial[] = {
        "You've been abducted by aliens, you have to work for them.",
        "You'll be tasked with coding apps for them.",
        "",
        "Press TAB to switch between the editor and console.",
        "Type \"help\" in the console to show the docs.",
        "",
        "Bugs will keep appearing, your builder will chase and kill them by default.",
        "Colliding with the bugs while the builder is going to build, will stop the build process.",
        "",
        "You'll be returned to this menu when you win or lose.",
    };
    int tutorialFontSize = 20;
    int tutorialLines = sizeof(tutorial) / sizeof(tutorial[0]);
    int tutorialY = (GetScreenHeight() - tutorialLines * (tutorialFontSize)) / 2;

    for (int i = 0; i < tutorialLines; i++) {
        Vector2 tutorialSize = MeasureTextEx(GetFontDefault(), tutorial[i], tutorialFontSize, 1);
        DrawText(tutorial[i], (GetScreenWidth() - tutorialSize.x - 70) / 2, tutorialY + i * (tutorialFontSize + 5), tutorialFontSize, WHITE);
    }
}
void menu_state_exit(void)
{
}