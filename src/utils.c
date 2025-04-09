#include "utils.h"
#include <math.h>

int fsign(float a)
{
    return a >= 0.0f ? 1 : -1;
}

Vector2 grid_to_world(Node grid_pos, int tile_size)
{
    return CLITERAL(Vector2){
        .x = grid_pos.x*tile_size,
        .y = grid_pos.y*tile_size
    };
}
Node world_to_grid_rounded(Vector2 world_pos, int tile_size)
{
    return CLITERAL(Node){
        .x = round(world_pos.x/tile_size),
        .y = round(world_pos.y/tile_size)
    };
}
Node world_to_grid(Vector2 world_pos, int tile_size)
{
    return CLITERAL(Node){
        .x = (int)(world_pos.x/tile_size),
        .y = (int)(world_pos.y/tile_size)
    };
}
Vector2 find_random_unblocked_neighbor_tile(Vector2 position, int* level_grid, int level_grid_width, int level_grid_height, int tile_size)
{
    // Convert the current position to grid coordinates
    Node current_grid_pos = world_to_grid(position, tile_size);
    
    // Array to hold the coordinates of neighboring tiles
    Node neighbors[8];
    int count = 0;

    // Check neighboring tiles (up, down, left, right)

    // Up, down, left and right neighbors
    if (current_grid_pos.x > 0 && level_grid[current_grid_pos.y * level_grid_width + (current_grid_pos.x - 1)] == 0) { // Left
        neighbors[count++] = (Node){current_grid_pos.x - 1, current_grid_pos.y};

        if (current_grid_pos.x > 0 && level_grid[current_grid_pos.y * level_grid_width + (current_grid_pos.x - 2)] == 0) { // Left*2
            neighbors[count++] = (Node){current_grid_pos.x - 2, current_grid_pos.y};
        }
    }
    if (current_grid_pos.x < level_grid_width - 1 && level_grid[current_grid_pos.y * level_grid_width + (current_grid_pos.x + 1)] == 0) { // Right
        neighbors[count++] = (Node){current_grid_pos.x + 1, current_grid_pos.y};

        if (current_grid_pos.x < level_grid_width - 1 && level_grid[current_grid_pos.y * level_grid_width + (current_grid_pos.x + 2)] == 0) { // Right*2
            neighbors[count++] = (Node){current_grid_pos.x + 2, current_grid_pos.y};
        }
    }
    if (current_grid_pos.y > 0 && level_grid[(current_grid_pos.y - 1) * level_grid_width + current_grid_pos.x] == 0) { // Up
        neighbors[count++] = (Node){current_grid_pos.x, current_grid_pos.y - 1};

        if (current_grid_pos.y > 0 && level_grid[(current_grid_pos.y - 2) * level_grid_width + current_grid_pos.x] == 0) { // Up*2
            neighbors[count++] = (Node){current_grid_pos.x, current_grid_pos.y - 2};
        }
    }
    if (current_grid_pos.y < level_grid_height - 1 && level_grid[(current_grid_pos.y + 1) * level_grid_width + current_grid_pos.x] == 0) { // Down
        neighbors[count++] = (Node){current_grid_pos.x, current_grid_pos.y + 1};

        if (current_grid_pos.y < level_grid_height - 1 && level_grid[(current_grid_pos.y + 2) * level_grid_width + current_grid_pos.x] == 0) { // Down*2
            neighbors[count++] = (Node){current_grid_pos.x, current_grid_pos.y + 2};
        }
    }

    // If there are no unblocked neighbors, return the original position
    if (count == 0) {
        return position;
    }

    // Select a random neighbor index using raylib's GetRandomValue
    int randomIndex = GetRandomValue(0, count - 1);

    // Select a random neighbor
    Node random_neighbor = neighbors[randomIndex];
    return grid_to_world(random_neighbor, tile_size);
}
bool is_line_obstructed(Vector2 start, Vector2 end, int* level_grid, int level_grid_width, int level_grid_height, int tile_size)
{
    // Convert start and end positions to grid coordinates
    Node start_grid = world_to_grid(start, tile_size);
    Node end_grid = world_to_grid(end, tile_size);

    // Bresenham's line algorithm
    int dx = abs(end_grid.x - start_grid.x);
    int dy = abs(end_grid.y - start_grid.y);
    int sx = start_grid.x < end_grid.x ? 1 : -1;
    int sy = start_grid.y < end_grid.y ? 1 : -1;
    int err = dx - dy;

    while (start_grid.x != end_grid.x || start_grid.y != end_grid.y) {
        // Check if the current tile is blocked
        int index = start_grid.y * level_grid_width + start_grid.x;
        if (index < 0 || index >= level_grid_width * level_grid_height) {
            // Out of bounds
            return true;
        }
        if (level_grid[index] == 1 || level_grid[index] > 2) {
            return true;
        }

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            start_grid.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            start_grid.y += sy;
        }
    }

    // Check the final tile
    int final_index = end_grid.y * level_grid_width + end_grid.x;
    if (final_index < 0 || final_index >= level_grid_width * level_grid_width) {
        // Out of bounds
        return true;
    }
    if (level_grid[final_index] == 1 || level_grid[final_index] > 2) {
        return true;
    }

    return false;
}