#include "pathfinding.h"
#include "global_context.h"
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

void init_priority_queue(PriorityQueue* pq, int capacity) {
    pq->nodes = malloc(sizeof(Node) * capacity);
    pq->priorities = malloc(sizeof(int) * capacity);
    pq->count = 0;
    pq->capacity = capacity;
}

void swap(Node* a, Node* b) {
    Node temp = *a;
    *a = *b;
    *b = temp;
}

void swap_int(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void push(PriorityQueue* pq, Node node, int priority) {
    if (pq->count < pq->capacity) {
        int i = pq->count++;
        pq->nodes[i] = node;
        pq->priorities[i] = priority;

        while (i > 0) {
            int parent = (i - 1) / 2;
            if (pq->priorities[parent] <= pq->priorities[i]) break;
            swap(&pq->nodes[i], &pq->nodes[parent]);
            swap_int(&pq->priorities[i], &pq->priorities[parent]);
            i = parent;
        }
    }
}

Node pop(PriorityQueue* pq) {
    Node result = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->count];
    pq->priorities[0] = pq->priorities[pq->count];

    int i = 0;
    while (1) {
        int left = 2 * i + 1, right = 2 * i + 2, min = i;
        if (left < pq->count && pq->priorities[left] < pq->priorities[min]) min = left;
        if (right < pq->count && pq->priorities[right] < pq->priorities[min]) min = right;
        if (min == i) break;
        swap(&pq->nodes[i], &pq->nodes[min]);
        swap_int(&pq->priorities[i], &pq->priorities[min]);
        i = min;
    }

    return result;
}

int heuristic(Node a, Node b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}


Path a_star(Node start, Node goal, int* grid, int width, int height, Arena* nodes_allocator) {
    PriorityQueue openSet;
    init_priority_queue(&openSet, width * height);
    push(&openSet, start, 0);

    int* cameFrom = (int*)arena_alloc(&g_context.frameArena, sizeof(int) * width * height);
    int* gScore = (int*)arena_alloc(&g_context.frameArena, sizeof(int) * width * height);
    int* fScore = (int*)arena_alloc(&g_context.frameArena, sizeof(int) * width * height);
    for (int i = 0; i < width * height; i++) {
        gScore[i] = fScore[i] = INT_MAX;
        cameFrom[i] = -1;
    }
    gScore[start.y * width + start.x] = 0;
    fScore[start.y * width + start.x] = heuristic(start, goal);

    while (openSet.count > 0) {
        Node current = pop(&openSet);
        if (current.x == goal.x && current.y == goal.y) {
            Path path;
            path.length = 0;
            path.nodes_allocation_size = sizeof(Node)*width*height;
            path.nodes = arena_alloc(nodes_allocator, sizeof(Node) * width * height);
            while (cameFrom[current.y * width + current.x] != -1) {
                path.nodes[path.length++] = current;
                int index = cameFrom[current.y * width + current.x];
                current.x = index % width;
                current.y = index / width;
            }
            path.nodes[path.length++] = start;
            return path;
        }

        Node neighbors[4] = {
            {current.x + 1, current.y}, {current.x - 1, current.y},
            {current.x, current.y + 1}, {current.x, current.y - 1}
        };

        for (int i = 0; i < 4; i++) {
            Node neighbor = neighbors[i];
            if (neighbor.x < 0 || neighbor.x >= width || neighbor.y < 0 || neighbor.y >= height) continue;
            if (grid[neighbor.y * width + neighbor.x] == 1 || grid[neighbor.y * width + neighbor.x] > 2) continue; 

            int tentative_gScore = gScore[current.y * width + current.x] + 1;
            if (tentative_gScore < gScore[neighbor.y * width + neighbor.x]) {
                cameFrom[neighbor.y * width + neighbor.x] = current.y * width + current.x;
                gScore[neighbor.y * width + neighbor.x] = tentative_gScore;
                fScore[neighbor.y * width + neighbor.x] = tentative_gScore + heuristic(neighbor, goal);
                push(&openSet, neighbor, fScore[neighbor.y * width + neighbor.x]);
            }
        }
    }

    Path empty_path = { .length = 0, .nodes_allocation_size = 0, .nodes = NULL };
    return empty_path;
}
