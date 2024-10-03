#include "pq.h"

#include <stdio.h>

typedef struct ListElement ListElement;

struct ListElement {
    Node *tree;
    ListElement *next;
};

struct PriorityQueue {
    ListElement *list;
};

PriorityQueue *pq_create(void) {
    PriorityQueue *pq = (PriorityQueue *) calloc(1, sizeof(PriorityQueue));
    if (pq == NULL) {
        fprintf(stderr, "Error: Allocation error.\n");
        return NULL;
    }

    return pq;
}

void pq_free(PriorityQueue **q) {
    free(*q);
    *q = NULL;
}

bool pq_is_empty(PriorityQueue *q) {
    if (q->list == NULL) {
        return true;
    }
    return false;
}

// bool pq_size_is_1(PriorityQueue *q) {
//     if(q ->list != NULL) {
//         return true;
//     }
//     return false;
// }

bool pq_size_is_1(PriorityQueue *q) {
    if (q->list != NULL && q->list->next == NULL) {
        return true;
    }
    return false;
}

// bool pq_size_is_1(PriorityQueue *q) {
//     if (pq_is_empty(q) == false) {
//         if ((q->list)->next == NULL) {
//             return true;
//         }
//     }
//     return false;
// }

bool pq_less_than(ListElement *e1, ListElement *e2) {
    if ((e1->tree->weight) < (e2->tree->weight)) {
        return true;
    }

    if ((e1->tree->weight) == (e2->tree->weight)) {
        return ((e1->tree->symbol) < (e2->tree->symbol));
    }

    return false;
}

void enqueue(PriorityQueue *q, Node *tree) {
    ListElement *new_element = (ListElement *) calloc(1, sizeof(ListElement));

    new_element->tree = tree;

    if (pq_is_empty(q)) {
        q->list = new_element;
    } else if (pq_less_than(new_element, q->list)) {
        new_element->next = q->list;
        q->list = new_element;
    } else {
        ListElement *current = q->list;

        while (current->next != NULL && pq_less_than(current->next, new_element)) {
            current = current->next;
        }

        new_element->next = current->next;
        current->next = new_element;
    }
}

Node *dequeue(PriorityQueue *q) {
    // Check if the queue is empty
    if (pq_is_empty(q)) {
        fprintf(stderr, "Error: Attempting to dequeue from an empty queue.\n");
        exit(EXIT_FAILURE);
    }

    // Retrieve the node with the lowest weight
    Node *dequeuedNode = q->list->tree;

    // Update the queue to exclude the dequeued node
    ListElement *temp = q->list;
    q->list = q->list->next;
    free(temp);

    // Return the dequeued node
    return dequeuedNode;
}

void pq_print(PriorityQueue *q) {
    assert(q != NULL);
    ListElement *e = q->list;
    int position = 1;

    while (e != NULL) {
        if (position++ == 1) {
            printf("=============================================\n");
        } else {
            printf("---------------------------------------------\n");
        }

        node_print_tree(e->tree);
        e = e->next;
    }

    printf("=============================================\n");
}
