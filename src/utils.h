#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

typedef struct Node
{
    int x, y;
} Node;

int fsign(float a);

Vector2 grid_to_world(Node grid_pos, int tile_size);
Node world_to_grid_rounded(Vector2 world_pos, int tile_size);
Node world_to_grid(Vector2 world_pos, int tile_size);

// Function to find and return a random, neighboring and unblocked position
Vector2 find_random_unblocked_neighbor_tile(Vector2 position, int* level_grid, int level_grid_width, int level_grid_height, int tile_size);

// Function to check if the line between two points is obstructed by a blocked tile
bool is_line_obstructed(Vector2 start, Vector2 end, int* level_grid, int level_grid_width, int level_grid_height, int tile_size);

#endif // UTILS_H