#ifndef SCRIPT_H
#define SCRIPT_H

#include <stddef.h>
#include <stdbool.h>
#include "raylib.h"

typedef struct Canvas
{
    char *name;
    int grid_size[2];
    char *background; // Using strings for simplicity
} Canvas;


typedef struct Block
{
    int grid_pos[2]; // Used by all blocks
    char* type;      // Used by all blocks
    char* path;      // Specific to the image
    char* label;     // Specific to the button
    char* action;    // Specific to the button
    char* content;   // Specific to the text
    char *color;     // Used by the button, and text
    Texture2D image_tex;
} Block;

typedef struct Structure // Houses the full generated structure of the script
{
    Canvas canvas;
    Block* blocks;
    size_t block_count;
} Structure;

typedef struct Script // Contains the generate script in string form
{
    char** script; // List of block text
    size_t block_count;
    char* description;
} Script;

Structure generate_structure(int min_grid_x, int max_grid_x, int min_grid_y, int max_grid_y);
Script structure_to_script(Structure* structure);
void generate_description(Script* script);

bool verify_block(Script s, size_t block_index, char* block_code);

void free_script(Structure structure, Script script);

#endif // SCRIPT_H