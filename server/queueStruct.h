#ifndef QUEUESTRUCT_H
#define QUEUESTRUCT_H

#include <pthread.h>

typedef struct Node {
    char* command;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
    pthread_mutex_t lock;
} Queue;

Queue* createQueue();
void enQueue(Queue* q, char* value);
char* deQueue(Queue* q);

#endif
