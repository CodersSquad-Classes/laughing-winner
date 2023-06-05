//queueStruct.c
#include "queueStruct.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

Queue* createQueue() {
    Queue* q = malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->lock, NULL);
    return q;
}

void enQueue(Queue* q, char* command) {
    Node* temp = malloc(sizeof(Node));
    temp->command = strdup(command);
    temp->next = NULL;

    pthread_mutex_lock(&q->lock);

    if (q->rear == NULL) {
        q->front = q->rear = temp;
    } else {
        q->rear->next = temp;
        q->rear = temp;
    }

    pthread_mutex_unlock(&q->lock);
}

char* deQueue(Queue* q) {
    pthread_mutex_lock(&q->lock);

    if (q->front == NULL) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    Node* temp = q->front;
    char* command = strdup(temp->command);

    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp->command);
    free(temp);

    pthread_mutex_unlock(&q->lock);

    return command;
}
