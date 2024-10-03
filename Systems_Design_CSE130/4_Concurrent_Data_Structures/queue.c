#include "queue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct queue {
    int front;
    int capacity;
    int rear;
    void **elements;
    pthread_cond_t pushCondition;
    pthread_cond_t popCondition;
    pthread_mutex_t mutex;

} queue_t;

// Function to create a new queue
queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL; // handle allocation failure
    }

    q->capacity = size;
    q->rear = 0;
    q->elements = (void **) malloc(size * sizeof(void *));
    if (q->elements == NULL) {
        free(q);
        return NULL; // handle allocation failure
    }

    pthread_cond_init(&q->popCondition, NULL);
    pthread_cond_init(&q->pushCondition, NULL);
    pthread_mutex_init(&q->mutex, NULL);

    // Initialize elements array
    for (int i = 0; i < size; i++) {
        q->elements[i] = NULL; // Initialize each element to NULL
    }

    return q;
}

//deletes queue instance
void queue_delete(queue_t **q) {
    if (q == NULL || *q == NULL) {
        return; // handle invalid input
    }

    pthread_mutex_destroy(&(*q)->mutex);
    pthread_cond_destroy(&(*q)->pushCondition);
    pthread_cond_destroy(&(*q)->popCondition);
    free(*q);
    *q = NULL;
}

//do dah push
bool queue_push(queue_t *q, void *elem) {
    pthread_mutex_lock(&q->mutex);
    if (q == NULL) {
        pthread_mutex_unlock(&q->mutex);
        return false; // Handle invalid queue input
    }

    // Wait if the queue is full
    while (q->rear >= q->capacity) {
        pthread_cond_wait(&q->popCondition, &q->mutex);
    }

    // Add the element at the rear position
    q->elements[q->rear] = elem;
    q->rear++;

    // Signal that an element has been pushed
    pthread_cond_signal(&q->pushCondition);
    pthread_mutex_unlock(&q->mutex);

    return true;
}

//do dah pop
bool queue_pop(queue_t *q, void **elem) {
    pthread_mutex_lock(&q->mutex);
    if (!q || elem == NULL) {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }

    // Wait if the queue is empty
    while (q->rear == 0) {
        pthread_cond_wait(&q->pushCondition, &q->mutex);
    }

    *elem = q->elements[q->front];

    // Shift elements in the queue
    for (int i = 0; i < q->rear - 1; i++) {
        q->elements[i] = q->elements[i + 1];
    }

    q->rear--;

    // Signal that an element has been popped
    pthread_cond_signal(&q->popCondition);
    pthread_mutex_unlock(&q->mutex);

    return true;
}
