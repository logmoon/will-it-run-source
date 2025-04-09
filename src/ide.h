#ifndef IDE_H
#define IDE_H

#include "string.h"
#include "raylib.h"
#include "script.h"
#include <stdbool.h>

typedef struct {
    size_t begin;
    size_t end;
} Line;

typedef struct CodeBlock
{
    String data;
    Line* lines;
    size_t lineCount;
    bool built;
    size_t cursor;
} CodeBlock;

typedef struct {
    String currentCommand;  // Current command being typed
    size_t cursor;          // Cursor position in the console
    bool isActive;          // Whether the console is active (focused)
} Console;

typedef struct {
    CodeBlock* codeBlocks;
    size_t currentCodeBlock;
    float scrollOffsetY;
    Console console;
} Editor;

Editor* ide_init(void);
void ide_update(Editor* editor, Rectangle bounds, Script script);
void ide_render(Editor* editor, Rectangle bounds);
void build_commit(Editor* editor);

#endif // IDE_H