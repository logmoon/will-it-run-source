#include "script.h"

#include "global_context.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int get_random_int()
{
    return GetRandomValue(0, 18290);
}

char* get_random_block_type()
{
    const char* block_types[] = {"image", "button", "text"};
    return strdup(block_types[get_random_int() % 3]);
}
char* get_random_color()
{
    const char* colors[] = {"RED", "GREEN", "BLUE", "WHITE", "YELLOW"};
    return strdup(colors[get_random_int() % 5]);
}
char* get_random_button_labels()
{
    const char* labels[] = {"Vorp?", "Inspect Human", "Zlorb Me", "Zip!", "Destroy Earth", "Turn to Zlurp Juice", "Zoko Ziko", "Launch Zorbital Beam", "Send Keplarian Ship"};
    return strdup(labels[get_random_int() % 9]);
}
char* get_random_button_action()
{
    const char* actions[] = {"on_zlorp", "on_gloop", "on_skibidi", "on_the_dead_vlorbians", "on_gnar", "on_blorb", "on_zip"};
    return strdup(actions[get_random_int() % 7]);
}
char* get_random_text_content()
{
    const char* content[] = {"Long live Zlorpus The Second", "What is adrenaline?", "Bogos Binted", "The Zlopps", "Keplerian soup vessel", "Zoko Ziko"};
    return strdup(content[get_random_int() % 6]);
}
char* get_random_app_name()
{
    const char* app_names[] = {
        "zip app",
        "glorp app",
        "vorp app",
        "grarp app",
        "glarp app",
        "zokoziko app",
        "gnar app",
        "florx app",
        "blorb app",
        "trax app",
        "plix app",
        "sloop app",
        "krong app",
        "morg app",
        "drillop app",
        "snarp app",
        "zlorb app",
        "klarn app",
        "noxplar app",
        "plux app",
        "thorp app",
        "worglax app",
        "zrik app",
        "vlorp app",
        "brakk app",
        "klox app",
        "tork app",
        "zogrun app",
        "krib app",
        "xlop app"
    };

    return strdup(app_names[get_random_int() % 30]);
}

Canvas generate_random_canvas(int min_grid_x, int max_grid_x, int min_grid_y, int max_grid_y)
{
    Canvas canvas;
    canvas.name = get_random_app_name();
    canvas.grid_size[0] = get_random_int() % max_grid_x + min_grid_x;
    canvas.grid_size[1] = get_random_int() % max_grid_y + min_grid_y;
    canvas.background = get_random_color();
    return canvas;
}
Block generate_random_block(Canvas canvas, int x, int y)
{
    Block block;
    block.grid_pos[0] = x;
    block.grid_pos[1] = y;

    char* c = get_random_color();
    while (strcmp(canvas.background, c) == 0)
    {
        c = get_random_color();
    }
    
    block.type = get_random_block_type();
    
    if (strcmp(block.type, "image") == 0)
    {
        block.path = strdup("path_to_image.png");
        block.image_tex = GetRandomTexture(TEXTURE_IB_1, TEXTURE_IB_7);
    }
    else if (strcmp(block.type, "button") == 0)
    {
        block.label = strdup(get_random_button_labels());
        block.color = c;
        block.action = strdup(get_random_button_action());
    }
    else if (strcmp(block.type, "text") == 0)
    {
        block.content = strdup(get_random_text_content());
        block.color = c;
    }
    
    return block;
}
Structure generate_structure(int min_grid_x, int max_grid_x, int min_grid_y, int max_grid_y)
{
    Structure structure;
    structure.canvas = generate_random_canvas(min_grid_x, max_grid_x, min_grid_y, max_grid_y);
    
    size_t max_blocks = structure.canvas.grid_size[0] * structure.canvas.grid_size[1];
    structure.blocks = malloc(max_blocks * sizeof(Block));
    structure.block_count = 0;
    
    bool** occupied = malloc(structure.canvas.grid_size[0] * sizeof(bool*));
    for (int i = 0; i < structure.canvas.grid_size[0]; i++) {
        occupied[i] = calloc(structure.canvas.grid_size[1], sizeof(bool));
    }
    
    for (int x = 0; x < structure.canvas.grid_size[0]; x++) {
        for (int y = 0; y < structure.canvas.grid_size[1]; y++) {

            structure.blocks[structure.block_count] = generate_random_block(structure.canvas, x, y);
            structure.block_count++;
            occupied[x][y] = true;
            
            if (structure.block_count >= max_blocks) {
                goto cleanup; // Exit both loops if we've filled the grid
            }
        }
    }
    
cleanup:
    for (int i = 0; i < structure.canvas.grid_size[0]; i++) {
        free(occupied[i]);
    }
    free(occupied);
    
    return structure;
}

Script structure_to_script(Structure* structure)
{
    Script script;
    script.block_count = structure->block_count + 1; // +1 for canvas
    script.script = malloc(script.block_count * sizeof(char*));
    
    // Generate canvas script
    char canvas_script[256];
    snprintf(canvas_script, sizeof(canvas_script), 
             "CANVAS\nname: %s\ngrid_size: %d,%d\nbackground: %s",
             structure->canvas.name, structure->canvas.grid_size[0], structure->canvas.grid_size[1], structure->canvas.background);
    script.script[0] = strdup(canvas_script);
    
    // Generate block scripts
    for (size_t i = 0; i < structure->block_count; i++) {
        Block* block = &structure->blocks[i];
        char block_script[256] = "";
        snprintf(block_script, sizeof(block_script),
                 "BLOCK\ngrid_pos: %d,%d\ntype: %s\n",
                 block->grid_pos[0], block->grid_pos[1], block->type);
        
        if (strcmp(block->type, "image") == 0)
        {
            snprintf(block_script + strlen(block_script), sizeof(block_script) - strlen(block_script),
                     "path: %s", block->path);
        }
        else if (strcmp(block->type, "button") == 0)
        {
            snprintf(block_script + strlen(block_script), sizeof(block_script) - strlen(block_script),
                     "label: %s\ncolor: %s\naction: %s", block->label, block->color, block->action);
        }
        else if (strcmp(block->type, "text") == 0)
        {
            snprintf(block_script + strlen(block_script), sizeof(block_script) - strlen(block_script),
                     "content: %s\ncolor: %s", block->content, block->color);
        }
        script.script[i + 1] = strdup(block_script);
    }
    
    return script;
}

// Function to generate description
void generate_description(Script* script)
{
    script->description = malloc(4096); // Allocate large enough buffer
    script->description[0] = '\0';  // Initialize as empty string

    // Process canvas information (first part of the script)
    char* canvas_info = script->script[0];
    char canvas_name[256];
    int grid_size_x, grid_size_y;
    char background_color[256];

    // Extract canvas details from the string
    sscanf(canvas_info, "CANVAS\nname: %255[^\n]\ngrid_size: %d,%d\nbackground: %255[^\n]",
           canvas_name, &grid_size_x, &grid_size_y, background_color);

    // Add the canvas script->description to the final output
    snprintf(script->description + strlen(script->description), 4096 - strlen(script->description),
             "Create an app called \"%s\", with a grid_size: {%d,%d} and background: %s.\n",
             canvas_name, grid_size_x, grid_size_y, background_color);

    // Iterate over each block and generate script->description
    for (size_t i = 1; i < script->block_count; i++)
    {
        char* block_info = script->script[i];
        int grid_pos_x, grid_pos_y;
        char block_type[256];

        // Extract block common details
        sscanf(block_info, "BLOCK\ngrid_pos: %d,%d\ntype: %255[^\n]", &grid_pos_x, &grid_pos_y, block_type);

        // Generate block-specific script->description based on type
        if (strcmp(block_type, "text") == 0)
        {
            char content[256], color[256];

            sscanf(block_info, "BLOCK\ngrid_pos: %d,%d\ntype: text\ncontent: %255[^\n]\ncolor: %255[^\n]",
                   &grid_pos_x, &grid_pos_y, content, color);

            snprintf(script->description + strlen(script->description), 4096 - strlen(script->description),
                     "- Put a text block that says: \"%s\" at position {%d,%d} with color: %s.\n",
                     content, grid_pos_x, grid_pos_y, color);

        }
        else if (strcmp(block_type, "button") == 0)
        {
            char label[256], color[256], action[256];
            
            sscanf(block_info, "BLOCK\ngrid_pos: %d,%d\ntype: button\nlabel: %255[^\n]\ncolor: %255[^\n]\naction: %255[^\n]",
                   &grid_pos_x, &grid_pos_y, label, color, action);

            snprintf(script->description + strlen(script->description), 4096 - strlen(script->description),
                     "- Place a button labeled \"%s\" at position {%d,%d} with color: %s and action: \"%s\".\n",
                     label, grid_pos_x, grid_pos_y, color, action);

        }
        else if (strcmp(block_type, "image") == 0)
        {
            char path[256];

            sscanf(block_info, "BLOCK\ngrid_pos: %d,%d\ntype: image\npath: %255[^\n]",
                   &grid_pos_x, &grid_pos_y, path);

            snprintf(script->description + strlen(script->description), 4096 - strlen(script->description),
                     "- Inset an image from \"%s\" at position {%d,%d}.\n",
                     path, grid_pos_x, grid_pos_y);
        }
    }
}
bool verify_block(Script s, size_t block_index, char* block_code)
{
    return strcmp(s.script[block_index], block_code) == 0;
}

void free_script(Structure structure, Script script)
{
    free(structure.blocks);
    free(script.description);
    for (size_t i = 0; i < script.block_count; i++) {
        free(script.script[i]);
    }
    free(script.script);
}