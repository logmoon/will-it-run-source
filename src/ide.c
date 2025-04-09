#include "ide.h"
#include "global_context.h"
#include "gameplay_state.h"
#include "state_manager.h"

#define INITIAL_DATA_SIZE 1000
#define MAX_CODE_BLOCKS 100
#define MAX_LINES 200

#define LINE_HEIGHT 20  // Height of each line in pixels

#define LINE_NUMBER_WIDTH 40  // Width of the area for line numbers

#define REPEAT_DELAY 0.55f   // Hold delay before repeating (in seconds)
#define REPEAT_RATE 0.06f   // Repeat interval (in seconds)

#define APP_BAR_HEIGHT 40   // Height of the app bar
#define SCROLL_BAR_WIDTH 5

#define CONSOLE_HEIGHT 100   // Height of the console area

#define BLOCK_PADDING 20
#define BLOCK_SPACING 10  // Space between code blocks

typedef struct KeyState
{
    bool isHeld;
    float holdTime;
    float lastRepeatTime;
} KeyState;

typedef struct KeyHoldStates
{
    KeyState up;
    KeyState down;
    KeyState left;
    KeyState right;
    KeyState backspace;
    KeyState tab;
    KeyState enter;
} KeyHoldStates;

KeyHoldStates keyHoldStates = {0};

// Handle input for a key press with hold and repeat functionality
void reset_key_state(KeyState* keyState)
{
    keyState->isHeld = false;
    keyState->holdTime = 0;
    keyState->lastRepeatTime = 0;
}
bool process_key_press(KeyState* keyState, int key)
{
    float frameTime = GetFrameTime(); // Time since the last frame
    keyState->holdTime += frameTime;

    if (IsKeyDown(key)) {
        if (!keyState->isHeld) {
            // Initial key press
            keyState->isHeld = true;
            keyState->lastRepeatTime = 0;
            return true;
        } else if (keyState->holdTime > REPEAT_DELAY) {
            // Repeat after the initial delay
            if (keyState->holdTime - keyState->lastRepeatTime >= REPEAT_RATE) {
                keyState->lastRepeatTime = keyState->holdTime;
                return true;
            }
        }
    } else {
        // Reset state when key is released
        reset_key_state(keyState);
    }

    return false;
}

Editor* ide_init(void)
{
    Editor* editor = arena_alloc(&g_context.stateArena, sizeof(Editor));

    editor->codeBlocks = arena_alloc(&g_context.stateArena, sizeof(CodeBlock) * MAX_CODE_BLOCKS);
    editor->currentCodeBlock = 0;
    editor->scrollOffsetY = 0;
    for (size_t i = 0; i < MAX_CODE_BLOCKS; i++)
    {
        editor->codeBlocks[i].cursor = 0;
        editor->codeBlocks[i].built = false;
        editor->codeBlocks[i].data = string_create_empty(INITIAL_DATA_SIZE);

        editor->codeBlocks[i].lines = arena_alloc(&g_context.stateArena, sizeof(Line) * MAX_LINES);

        editor->codeBlocks[i].lineCount = 1;
        editor->codeBlocks[i].lines[0].begin = 0;
        editor->codeBlocks[i].lines[0].end = 0;
    }

    editor->console.currentCommand = string_create_empty(INITIAL_DATA_SIZE);
    editor->console.cursor = 0;
    editor->console.isActive = false;

    return editor;
}

// Recalculate lines after a change in the `data` string
void update_lines(CodeBlock* codeBlock)
{
    size_t lineIndex = 0;
    size_t lineStart = 0;

    for (size_t i = 0; i < codeBlock->data.count; i++) {
        if (codeBlock->data.data[i] == '\n') {
            codeBlock->lines[lineIndex].begin = lineStart;
            codeBlock->lines[lineIndex].end = i;
            lineIndex++;
            lineStart = i + 1;
        }
    }

    // Handle the last line
    codeBlock->lines[lineIndex].begin = lineStart;
    codeBlock->lines[lineIndex].end = codeBlock->data.count;
    codeBlock->lineCount = lineIndex + 1;
}

// Calculate the total height of all blocks up to the current one
size_t calculate_total_height(Editor* editor)
{
    size_t totalHeight = 0;
    for (size_t i = 0; i <= editor->currentCodeBlock; i++)
    {
        totalHeight += (editor->codeBlocks[i].lineCount * LINE_HEIGHT) + (2 * BLOCK_PADDING) + (i > 0 ? BLOCK_SPACING : 0);
    }
    return totalHeight;
}
void ide_handle_console(Editor* editor, Rectangle editor_bounds, Script script)
{
    // Handle console input
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) {
            PlaySound(gameplay_data.type_sfx);
            string_append_char_at_index(&editor->console.currentCommand, (char)key, editor->console.cursor);
            editor->console.cursor++;
        }
        key = GetCharPressed();
    }

    // Handle special keys for console
    if (process_key_press(&keyHoldStates.backspace, KEY_BACKSPACE) && editor->console.cursor > 0)
    {
        PlaySound(gameplay_data.type_sfx);
        string_remove_char(&editor->console.currentCommand, editor->console.cursor - 1);
        editor->console.cursor--;
    }
    if (process_key_press(&keyHoldStates.enter, KEY_ENTER))
    {
        if (editor->console.currentCommand.data != NULL)
        {
            if (strcmp("build", editor->console.currentCommand.data) == 0)
            {
                if (verify_block(script, editor->currentCodeBlock, editor->codeBlocks[editor->currentCodeBlock].data.data))
                {
                    PlaySound(gameplay_data.build_success_sfx);
                    gameplay_data.build = true;
                }
                else
                {
                    PlaySound(gameplay_data.build_error_sfx);
                    printf("%s\n", "Error");
                }
            }
            else if (strcmp("help", editor->console.currentCommand.data) == 0)
            {
                PlaySound(gameplay_data.job_popup_sfx);
                gameplay_data.docs_popup = true;
            }
            else if (strcmp("job", editor->console.currentCommand.data) == 0)
            {
                PlaySound(gameplay_data.job_popup_sfx);
                gameplay_data.job_popup = true;
            }
            else if (strcmp("exit", editor->console.currentCommand.data) == 0)
            {
                change_state(STATE_MENU_STATE);
            }

            string_destroy(&editor->console.currentCommand);
            editor->console.cursor = 0;

            editor->console.isActive = !editor->console.isActive;

            // Scroll the view if possible
            size_t totalHeight = calculate_total_height(editor);
            size_t height = editor_bounds.height - APP_BAR_HEIGHT - (2 * BLOCK_PADDING);
            if (totalHeight > height)
            {
                editor->scrollOffsetY = totalHeight - height;
            }
        }
    }
}
void build_commit(Editor* editor)
{
    PlaySound(gameplay_data.build_success_sfx);
    editor->codeBlocks[editor->currentCodeBlock].built = true;
    editor->currentCodeBlock++;
    gameplay_data.build = false;
    gameplay_data.game_timer -= 30.0f;
}

void ide_handle_editor(Editor* editor, Rectangle bounds)
{
    CodeBlock* currentBlock = &editor->codeBlocks[editor->currentCodeBlock];
    
    // Calculate the total height of all blocks
    size_t totalHeight = calculate_total_height(editor);
    
    // Calculate the maximum width for text in a block
    float maxTextWidth = bounds.width - LINE_NUMBER_WIDTH - (3 * BLOCK_PADDING) - 20;

    // Handle character input
    int key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 125) // Allow printable characters
        {
            Line currentLine = currentBlock->lines[currentBlock->lineCount - 1];
            float currentLineWidth = MeasureText(TextSubtext(currentBlock->data.data, currentLine.begin, currentLine.end - currentLine.begin), 20);

            // Check if adding a character would exceed the block's width
            if (currentLineWidth < maxTextWidth) {
                PlaySound(gameplay_data.type_sfx);
                string_append_char_at_index(&currentBlock->data, (char)key, currentBlock->cursor);
                currentBlock->cursor++;
                update_lines(currentBlock);
            }
        }
        key = GetCharPressed();
    }

    // Handle special keys
    if (process_key_press(&keyHoldStates.backspace, KEY_BACKSPACE) && currentBlock->cursor > 0)
    {
        PlaySound(gameplay_data.type_sfx);
        string_remove_char(&currentBlock->data, currentBlock->cursor - 1);
        currentBlock->cursor--;
        update_lines(currentBlock);
    }
    if (process_key_press(&keyHoldStates.enter, KEY_ENTER))
    {
        PlaySound(gameplay_data.type_sfx);
        string_append_char_at_index(&currentBlock->data, '\n', currentBlock->cursor);
        currentBlock->cursor++;
        update_lines(currentBlock);
    }
    if (process_key_press(&keyHoldStates.right, KEY_RIGHT) && currentBlock->cursor < currentBlock->data.count)
    {
        PlaySound(gameplay_data.type_sfx);
        currentBlock->cursor++;
    }
    if (process_key_press(&keyHoldStates.left, KEY_LEFT) && currentBlock->cursor > 0)
    {
        PlaySound(gameplay_data.type_sfx);
        currentBlock->cursor--;
    }
    if (process_key_press(&keyHoldStates.down, KEY_DOWN))
    {
        PlaySound(gameplay_data.type_sfx);
        // Move cursor down a line
        for (size_t i = 0; i < currentBlock->lineCount - 1; i++)
        {
            if (currentBlock->cursor >= currentBlock->lines[i].begin && currentBlock->cursor <= currentBlock->lines[i].end)
            {
                size_t newLineIndex = i + 1;
                currentBlock->cursor = currentBlock->lines[newLineIndex].begin + (currentBlock->cursor - currentBlock->lines[i].begin);

                if (currentBlock->cursor > currentBlock->lines[newLineIndex].end)
                {
                    currentBlock->cursor = currentBlock->lines[newLineIndex].end;
                }
                break;
            }
        }
    }
    if (process_key_press(&keyHoldStates.up, KEY_UP))
    {
        PlaySound(gameplay_data.type_sfx);
        // Move cursor up a line
        for (size_t i = 1; i < currentBlock->lineCount; i++)
        {
            if (currentBlock->cursor >= currentBlock->lines[i].begin && currentBlock->cursor <= currentBlock->lines[i].end)
            {
                size_t newLineIndex = i - 1;
                currentBlock->cursor = currentBlock->lines[newLineIndex].begin + (currentBlock->cursor - currentBlock->lines[i].begin);

                if (currentBlock->cursor > currentBlock->lines[newLineIndex].end)
                {
                    currentBlock->cursor = currentBlock->lines[newLineIndex].end;
                }
                break;
            }
        }
    }

    // Handle scrolling with mouse wheel
    float mouseWheelMove = GetMouseWheelMove();
    size_t height = bounds.height - APP_BAR_HEIGHT - (2 * BLOCK_PADDING);
    if (totalHeight > height)
    {
        editor->scrollOffsetY -= mouseWheelMove * LINE_HEIGHT;
    }

    // Clamp scrolling to valid bounds
    if (editor->scrollOffsetY < 0)
    {
        editor->scrollOffsetY = 0;
    }
    if (editor->scrollOffsetY > totalHeight - height)
    {
        editor->scrollOffsetY = totalHeight - height;
    }
}


void ide_update(Editor* editor, Rectangle bounds, Script script)
{
    Rectangle ideBounds = bounds;
    ideBounds.height -= CONSOLE_HEIGHT;

    // Check for console activation
    if (IsKeyPressed(KEY_TAB))
    {
        PlaySound(gameplay_data.type_sfx);
        editor->console.isActive = !editor->console.isActive;
    }

    if (editor->console.isActive)
    {
        ide_handle_console(editor, ideBounds, script);
    }
    else
    {
        ide_handle_editor(editor, ideBounds);
    }
}

// Render the editor within the specified bounds
void ide_render_editor(Editor* editor, Rectangle bounds)
{
    int startX = bounds.x + LINE_NUMBER_WIDTH + (2 * BLOCK_PADDING);
    int startY = bounds.y + APP_BAR_HEIGHT;

    // Draw the background
    Texture2D bg_tex = g_context.textures[TEXTURE_INTERFACE_BG];
    DrawTexturePro(bg_tex,
                   (Rectangle){ 0, 0, bg_tex.width, bg_tex.height },
                   bounds,
                   (Vector2){ 0, 0 }, 0, WHITE);
    
    float totalBlockHeight = 0;
    for (size_t blockIndex = 0; blockIndex <= editor->currentCodeBlock; blockIndex++)
    {
        CodeBlock* block = &editor->codeBlocks[blockIndex];
        
        // Calculate the height of the block based on the number of lines, including padding
        int blockHeight = (block->lineCount * LINE_HEIGHT) + (2 * BLOCK_PADDING);
        
        // Calculate the position of the block, including the new spacing
        int blockTopY = startY + totalBlockHeight - editor->scrollOffsetY + BLOCK_PADDING;
        
        // Skip blocks that are above the visible area
        if (blockTopY + blockHeight < bounds.y + APP_BAR_HEIGHT) {
            totalBlockHeight += blockHeight + BLOCK_SPACING;  // Add spacing to total height
            continue;
        }
        
        // Skip blocks that are completely below the visible area
        if (blockTopY > bounds.y + bounds.height) {
            break;
        }
        
        // Draw a rectangle around the block, including padding
        Color blockColor = block->built ? LIGHTGRAY : RAYWHITE;
        DrawRectangle(bounds.x + BLOCK_PADDING, blockTopY, bounds.width - (2 * BLOCK_PADDING), blockHeight, blockColor);
        DrawRectangleLines(bounds.x + BLOCK_PADDING, blockTopY, bounds.width - (2 * BLOCK_PADDING), blockHeight, DARKGRAY);
        
        // Render line numbers and text for the block
        for (size_t i = 0; i < block->lineCount; i++)
        {
            Line line = block->lines[i];
            int posY = blockTopY + BLOCK_PADDING + (i * LINE_HEIGHT);

            // Skip lines that are outside the visible bounds
            if (posY + LINE_HEIGHT < bounds.y + APP_BAR_HEIGHT || posY > bounds.y + bounds.height) {
                continue;
            }

            // Render line number
            DrawText(TextFormat("%zu", i + 1), startX - LINE_NUMBER_WIDTH, posY, 20, DARKGRAY);

            // Render text within the block's rectangle
            DrawText(TextSubtext(block->data.data, line.begin, line.end - line.begin), startX, posY, 20, BLACK);
        }

        // Render the cursor for the current block
        if (blockIndex == editor->currentCodeBlock && !editor->console.isActive)
        {
            size_t cursorLineIndex = 0;
            for (size_t i = 0; i < block->lineCount; i++)
            {
                if (block->cursor >= block->lines[i].begin && block->cursor <= block->lines[i].end)
                {
                    cursorLineIndex = i;
                    break;
                }
            }
        
            // Calculate cursor position
            Line cursorLine = block->lines[cursorLineIndex];
            int cursorX = startX + MeasureText(TextSubtext(block->data.data, cursorLine.begin, block->cursor - cursorLine.begin), 20);
            int cursorY = blockTopY + BLOCK_PADDING + (cursorLineIndex * LINE_HEIGHT);

            // Render cursor as a vertical line if it's within the visible bounds
            if (cursorY + LINE_HEIGHT >= startY && cursorY <= bounds.y + bounds.height) {
                DrawLine(cursorX, cursorY, cursorX, cursorY + LINE_HEIGHT, BLACK);
            }
        }
        
        totalBlockHeight += blockHeight + BLOCK_SPACING;  // Add spacing to total height
    }

    // Render the scroll bar
    size_t totalHeight = totalBlockHeight - BLOCK_SPACING;  // Subtract last spacing
    size_t height = bounds.height - APP_BAR_HEIGHT;
    if (totalHeight > height)
    {
        DrawRectangle(bounds.width - SCROLL_BAR_WIDTH, bounds.y + APP_BAR_HEIGHT, SCROLL_BAR_WIDTH, height, WHITE);
        DrawRectangle(
            bounds.width - SCROLL_BAR_WIDTH,
            bounds.y + APP_BAR_HEIGHT + (int)((float)editor->scrollOffsetY / (float)totalHeight * height),
            SCROLL_BAR_WIDTH,
            (int)((float)height / (float)totalHeight * height),
            BLACK
        );
    }

    // Render the app bar
    DrawRectangle(bounds.x, bounds.y, bounds.width, APP_BAR_HEIGHT, DARKGRAY);
    DrawText("VZ Vorp Editor", bounds.x + 10, bounds.y + 10, 20, WHITE);
}
void ide_render_console(Editor* editor, Rectangle bounds)
{
    // Render the console at the bottom
    DrawRectangleRec(bounds, DARKGRAY);  // Console background
    DrawText("Console:", bounds.x + 10, bounds.y + 10, 20, WHITE);

    // Render the current console command
    DrawText("vorbiluis_pc >  ", bounds.x + 10, bounds.y + 40, 20, WHITE);
    int x_offset = MeasureText("vorbiluis_pc >  ", 20) + 10;
    DrawText(editor->console.currentCommand.data, x_offset, bounds.y + 40, 20, WHITE);

    if (editor->console.isActive)
    {
        // Render the console cursor
        int consoleCursorX = bounds.x + MeasureText(editor->console.currentCommand.data, 20) + x_offset;
        DrawLine(consoleCursorX, bounds.y + 40, consoleCursorX, bounds.y + 60, WHITE);
    }
}

void ide_render(Editor* editor, Rectangle bounds)
{
    // Split bounds into IDE and console
    Rectangle ideBounds = bounds;
    ideBounds.height -= CONSOLE_HEIGHT;
    ide_render_editor(editor, ideBounds);

    Rectangle consoleBounds = {bounds.x, bounds.y + ideBounds.height, bounds.width, CONSOLE_HEIGHT};
    ide_render_console(editor, consoleBounds);
}