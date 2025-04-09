#include "popup.h"
#include "raylib.h"
#include "global_context.h"
#include "gameplay_state.h"
#include <string.h>

#define MAX_LINES 50
#define MAX_LINE_LENGTH 200

typedef struct {
    const char* text;
    bool isHeader;
} DocLine;

void popup_render_docs()
{
    DocLine docs[] = {
        {"Vorp Lang Docs:", true},
        {"", false},
        {"Structure:", true},
        {"Every app must start with a CANVAS block", false},
        {"followed by multiple BLOCK elements.", false},
        {"", false},
        {"Canvas", true},
        {"Syntax:", false},
        {"CANVAS", false},
        {"name: APP_NAME", false},
        {"grid_size: SIZE_X,SIZE_Y", false},
        {"background: COLOR", false},
        {"", false},
        {"Button", true},
        {"Syntax:", false},
        {"BLOCK", false},
        {"grid_pos: POS_X,POS_Y", false},
        {"type: button", false},
        {"label: LABEL", false},
        {"color: COLOR", false},
        {"action: ACTION_NAME", false},
        {"", false},
        {"Image", true},
        {"Syntax:", false},
        {"BLOCK", false},
        {"grid_pos: POS_X,POS_Y", false},
        {"type: image", false},
        {"path: PATH_TO_IMAGE", false},
        {"", false},
        {"Text", true},
        {"Syntax:", false},
        {"BLOCK", false},
        {"grid_pos: POS_X,POS_Y", false},
        {"type: text", false},
        {"content: CONTENT", false},
        {"color: COLOR", false},
        {"", false},
        {"Notes", true},
        {"- The order of attributes within each block must remain THE SAME.", false},
        {"- The canvas must be defined before any blocks.", false},
        {"- Make sure to respect the spacing, new lines, and punctuation.", false},
    };

    int numLines = sizeof(docs) / sizeof(docs[0]);

    // Set up colors and sizes
    Color textColor = BLACK;
    Color headerColor = DARKGRAY;
    int fontSize = 10;
    int headerFontSize = 20;
    int lineHeight = fontSize + 5;
    int padding = 10;

    // Calculate the size and position of the docs window
    int docsWidth = SCREEN_WIDTH * 0.9;  // 80% of screen width
    int docsHeight = SCREEN_HEIGHT * 0.95;  // 90% of screen height
    int docsX = (SCREEN_WIDTH - docsWidth) / 2;
    int docsY = (SCREEN_HEIGHT - docsHeight) / 2;

    // Draw the background
    Texture2D bg_tex = g_context.textures[TEXTURE_POPUP_BG];
    DrawTexturePro(bg_tex,
                   (Rectangle){ 0, 0, bg_tex.width, bg_tex.height },
                   (Rectangle){ docsX, docsY, docsWidth, docsHeight },
                   (Vector2){ 0, 0 }, 0, WHITE);

    // Calculate column width and positions
    int numColumns = 2;
    int columnWidth = (docsWidth - (numColumns + 1) * padding) / numColumns;
    int column1X = docsX + padding;
    int column2X = column1X + columnWidth + padding;

    // Draw the content
    int currentX = column1X;
    int currentY = docsY + padding;
    int linesInCurrentColumn = 0;
    int maxLinesPerColumn = 22;  // Adjust this value to fit your needs

    for (int i = 0; i < numLines; i++) {
        if (linesInCurrentColumn >= maxLinesPerColumn) {
            currentX = column2X;
            currentY = docsY + padding;
            linesInCurrentColumn = 0;
        }

        if (docs[i].isHeader) {
            DrawText(docs[i].text, currentX, currentY, headerFontSize, headerColor);
            currentY += headerFontSize + 5;
        } else {
            DrawText(docs[i].text, currentX, currentY, fontSize, textColor);
            currentY += lineHeight;
        }

        linesInCurrentColumn++;
    }

    // Draw console commands section
    int consoleY = docsY + docsHeight - 100;
    DrawRectangle(docsX, consoleY, docsWidth, 100, LIGHTGRAY);
    DrawText("Console Commands:", docsX + padding, consoleY + padding, headerFontSize, headerColor);
    // Add your console commands here
    DrawText("build     - Builds the current block.", docsX + padding, consoleY + padding + 10 + lineHeight, fontSize, textColor);
    DrawText("help     - Display the docs screen.", docsX + padding, consoleY + padding + 10 + lineHeight * 2, fontSize, textColor);
    DrawText("job      - Shows the current job description.", docsX + padding, consoleY + padding + 10 + lineHeight * 3, fontSize, textColor);
    DrawText("exit     - Exits to main menu", docsX + padding, consoleY + padding + 10 + lineHeight * 4, fontSize, textColor);

    DrawText("Press TAB to close", docsX + docsWidth - 250, docsY + 20, 20, RED);
}
void popup_render_job(Editor editor, Script script)
{
    // Set up colors and sizes
    Color textColor = BLACK;
    Color headerColor = DARKGRAY;
    int fontSize = 20;
    int headerFontSize = 24;
    int padding = 20;

    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calculate the size and position of the popup window
    int popupWidth = screenWidth * 0.8;
    int popupHeight = screenHeight * 0.8;
    int popupX = (screenWidth - popupWidth) / 2;
    int popupY = (screenHeight - popupHeight) / 2;

    // Draw the background
    Texture2D bg_tex = g_context.textures[TEXTURE_POPUP_BG];
    DrawTexturePro(bg_tex,
                   (Rectangle){ 0, 0, bg_tex.width, bg_tex.height },
                   (Rectangle){ popupX, popupY, popupWidth, popupHeight },
                   (Vector2){ 0, 0 }, 0, WHITE);

    // Load and draw the image
    Texture2D jobImage = gameplay_data.current_alien_texture;
    int imageWidth = popupWidth * 0.3;
    int imageHeight = popupHeight * 0.4;
    int imageX = popupX + (popupWidth - imageWidth) / 2;
    int imageY = popupY + padding;
    DrawTexturePro(jobImage,
                   (Rectangle){ 0, 0, jobImage.width, jobImage.height },
                   (Rectangle){ imageX, imageY, imageWidth, imageHeight },
                   (Vector2){ 0, 0 }, 0, WHITE);

    // Draw the job description header
    int descriptionY = imageY + imageHeight + padding + 20;
    DrawText("Job Description:", popupX + padding, descriptionY, headerFontSize, headerColor);

    // Find the current line to type
    const char* description = script.description;
    size_t descriptionLength = strlen(description);
    size_t startIndex = 0;
    size_t lineCount = 0;
    char currentLine[MAX_LINE_LENGTH + 1] = "";

    while (startIndex < descriptionLength && lineCount <= editor.currentCodeBlock) {
        size_t endIndex = startIndex;
        size_t lineLength = 0;

        // Find the end of the current line
        while (endIndex < descriptionLength && lineLength < MAX_LINE_LENGTH) {
            if (description[endIndex] == '\n') {
                break;
            }
            endIndex++;
            lineLength++;
        }

        // If this is the current line, save it
        if (lineCount == editor.currentCodeBlock) {
            strncpy(currentLine, description + startIndex, lineLength);
            currentLine[lineLength] = '\0';
            break;
        }

        // Move to the next line
        startIndex = endIndex + 1;
        lineCount++;
    }

    // Draw the current line to type with word wrapping
    int maxLineWidth = popupWidth - 2 * padding;
    int lineY = descriptionY + headerFontSize + 20;
    char* word = strtok(currentLine, " ");
    char lineBuffer[MAX_LINE_LENGTH + 1] = "";

    while (word != NULL) {
        char tempBuffer[MAX_LINE_LENGTH + 1];
        snprintf(tempBuffer, sizeof(tempBuffer), "%s%s ", lineBuffer, word);

        if (MeasureText(tempBuffer, fontSize) > maxLineWidth) {
            // Draw the current line and start a new one
            DrawText(lineBuffer, popupX + padding, lineY, fontSize, textColor);
            lineY += fontSize + 10;
            strncpy(lineBuffer, word, sizeof(lineBuffer) - 1);
            strncat(lineBuffer, " ", sizeof(lineBuffer) - strlen(lineBuffer) - 1);
        } else {
            // Add the word to the current line
            strncpy(lineBuffer, tempBuffer, sizeof(lineBuffer) - 1);
        }

        word = strtok(NULL, " ");
    }

    // Draw any remaining text
    if (strlen(lineBuffer) > 0) {
        DrawText(lineBuffer, popupX + padding, lineY, fontSize, textColor);
    }

    // Draw "Press TAB to close" text
    DrawText("Press TAB to close",
             popupX + (popupWidth - MeasureText("Press TAB to close", fontSize)) / 2,
             popupY + popupHeight - padding - fontSize,
             fontSize, RED);
}