#include "game_app.h"
#include "global_context.h"
#include "gameplay_state.h"
#include <string.h>
#include <math.h>

#define APP_BAR_HEIGHT 40

Color get_color_from_string(char* color)
{
    if (strcmp("RED", color) == 0)
    {
        return RED;
    }
    else if (strcmp("GREEN", color) == 0)
    {
        return GREEN;
    }
    else if (strcmp("BLUE", color) == 0)
    {
        return BLUE;
    }
    else if (strcmp("YELLOW", color) == 0)
    {
        return YELLOW;
    }
    else if (strcmp("WHITE", color) == 0)
    {
        return WHITE;
    }

    printf("Color: %s\n", color);
    return MAGENTA;
}

#define APP_BAR_HEIGHT 40
// Helper function to draw centered text with auto-scaling
void DrawTextCentered(const char *text, Rectangle rect, int fontSize, Color color)
{
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    float scaleFactor = fmin(rect.width / textSize.x, rect.height / textSize.y);
    int scaledFontSize = (int)(fontSize * scaleFactor);
    textSize = MeasureTextEx(GetFontDefault(), text, scaledFontSize, 1);
    
    Vector2 position = {
        rect.x + (rect.width - textSize.x) / 2,
        rect.y + (rect.height - textSize.y) / 2
    };
    
    DrawTextEx(GetFontDefault(), text, position, scaledFontSize, 1, color);
}
void game_app_render(Rectangle bounds, Editor editor, Structure structure)
{
    // Drawing app bar
    DrawRectangle(bounds.x, bounds.y, bounds.width, APP_BAR_HEIGHT, DARKBLUE);
    if ((int)editor.currentCodeBlock < 0 || !editor.codeBlocks[0].built) return;
    
    // Setup the canvas
    DrawText(structure.canvas.name, bounds.x + 10, bounds.y + 10, 20, BLACK);
    DrawRectangle(bounds.x, bounds.y + APP_BAR_HEIGHT, bounds.width, bounds.height - APP_BAR_HEIGHT, get_color_from_string(structure.canvas.background));
    int block_size_x = bounds.width / structure.canvas.grid_size[0];
    int block_size_y = (bounds.height - APP_BAR_HEIGHT) / structure.canvas.grid_size[1];
    
    // Drawing blocks
    for (size_t i = 0; i < structure.block_count; i++)
    {
        if (editor.codeBlocks[i+1].built)
        {
            Block* b = &structure.blocks[i];
            float x = bounds.x + (b->grid_pos[0] * block_size_x);
            float y = bounds.y + APP_BAR_HEIGHT + (b->grid_pos[1] * block_size_y);
            Rectangle blockRect = { x, y, block_size_x, block_size_y };
            
            if (strcmp(b->type, "image") == 0)
            {
                Texture2D t = b->image_tex;
                Rectangle sourceRec = { 0.0f, 0.0f, (float)t.width, (float)t.height };
                DrawTexturePro(t, sourceRec, blockRect, CLITERAL(Vector2){0.0f,0.0f}, 0.0f, WHITE);
            }
            else if (strcmp(b->type, "button") == 0)
            {
                // Draw button with 3D effect
                Color buttonColor = get_color_from_string(b->color);
                Color lightColor = ColorBrightness(buttonColor, 0.3f);
                Color darkColor = ColorBrightness(buttonColor, 0.3f);
                
                DrawRectangleRec(blockRect, buttonColor);
                DrawRectangleLinesEx(blockRect, 2, lightColor);
                DrawLine(x, y + block_size_y, x + block_size_x, y + block_size_y, darkColor);
                DrawLine(x + block_size_x, y, x + block_size_x, y + block_size_y, darkColor);
                
                DrawTextCentered(b->label, blockRect, 20, BLACK);
            }
            else if (strcmp(b->type, "text") == 0)
            {
                Color textColor = get_color_from_string(b->color);
                DrawTextCentered(b->content, blockRect, 20, textColor);
            }
        }
    }
}