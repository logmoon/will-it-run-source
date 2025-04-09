#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "raylib.h"
#include "arena.h"
#include "utils.h"

typedef struct {
    Node* nodes;
    int* priorities;
    int count;
    int capacity;
} PriorityQueue;

typedef struct {
    Node* nodes;
    size_t nodes_allocation_size;
    int length;
} Path;

Path a_star(Node start, Node goal, int* grid, int width, int height, Arena* nodes_allocator);
void init_priority_queue(PriorityQueue* pq, int capacity);
void push(PriorityQueue* pq, Node node, int priority);
Node pop(PriorityQueue* pq);
int heuristic(Node a, Node b);

#endif // PATHFINDING_H