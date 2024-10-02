/*******************************************************
* Name: Tristan Garcia
* CruzID: Tgarcia7
* StudentID: 1991617
* File: List.c
* ADT file for the List Functions
********************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "List.h"

/*********** Structs ***********/
typedef struct NodeObj* Node;

typedef struct NodeObj {
    int data;
    Node prev;
    Node next;
} NodeObj;


typedef struct ListObj {
    int length;
    int index;
    Node front;
    Node back;
    Node cursor;
} ListObj;

/*********** Constructors Destructors ***********/
//Creates new node and returns reference to new node object

Node newNode(int data) {
    Node N = malloc(sizeof(NodeObj));
    assert(N != NULL);
    N -> data = data;
    N -> next = NULL;
    return(N);
}

void freeNode(Node *pN) {
    if(pN != NULL && *pN != NULL) {
        free(*pN);
        *pN = NULL;
    }
}

List newList(void) {  
    List L;
    L = malloc(sizeof(ListObj));
    assert(L != NULL);
    L -> front = L -> back = L -> cursor = NULL;
    L -> length = 0;
    L -> index = -1;
    return(L);
}      

void freeList(List* pL) { 
    if(pL != NULL && *pL != NULL) {
        while(length(*pL) != 0) {
            deleteFront(*pL);
        }
        free(*pL);
        *pL = NULL;
    }
}

/*********** Access Functions ***********/
int length(List L) {
    if(L == NULL) {
        fprintf(stderr, "List error: calling length() on NULL list");
        exit(EXIT_FAILURE);
    }

    return(L -> length);
}

int index(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling index() on NULL list");
        exit(EXIT_FAILURE);
    }

    return(L -> index);
}

int front(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling front() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List error: calling front() on an empty list");
        exit(EXIT_FAILURE);
    }

    return(L -> front -> data);
}

int back(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling back() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling back() on empty list");
        exit(EXIT_FAILURE);
    }

    return(L -> back -> data);
}

int get(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling get() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> index < 0) {
        fprintf(stderr, "List Error: calling get() on empty index");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling get() on empty list");
        exit(EXIT_FAILURE);
    }

    return(L -> cursor -> data);
} 

bool equals(List A, List B) {
    if(A == NULL || B == NULL) {
        fprintf(stderr, "List Error: calling equals() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(A -> length != B -> length) {
        return false;
    }

    Node N, M;
    bool eq;

    eq = (A -> length == B -> length);
    N = A -> front;
    M = B -> front;

    while(N != NULL) {
        eq = (N -> data == M -> data);
        N = N -> next;
        M = M -> next;
    }

    return eq;
}

/*********** Manipulation procedures ***********/
void clear(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling clear() on NULL list");
        exit(EXIT_FAILURE);
    }

    while (L->length > 0) {
        deleteFront(L);
    }

    L -> front = L -> back = L -> cursor = NULL;
    L -> length = 0;
    L -> index = -1;
}

void set(List L, int x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling set() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling set() on empty list");
        exit(EXIT_FAILURE);
    }

    if(L -> index < 0) {
        fprintf(stderr, "List Error: calling set() on empty index");
        exit(EXIT_FAILURE);
    }

    L -> cursor -> data = x;
}

void moveFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveFront() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length > 0) {
        L -> cursor = L -> front;
        L -> index = 0;
    }

} 

void moveBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveBack() on NULL list");
        exit(EXIT_FAILURE);
    }

    if (L -> length <= 0) {
        fprintf(stderr, "List Error: calling moveBack() on empty list");
        exit(EXIT_FAILURE);
    }

    if(L -> length > 0) {
        L -> cursor = L -> back;
        L -> index = L -> length -1;
    }   
}  

void movePrev(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling movePrev() on NULL list");
        exit(EXIT_FAILURE);
    }

    //if cursor is defined and not at front
    if(L -> cursor != NULL && L -> index != 0) {
        L -> cursor = L -> cursor -> prev;
        L -> index--;
    }

    //if cursor is defined and at front
   else if(L -> cursor != NULL && L -> index == 0) {
        L -> cursor = NULL;
        L -> index = -1;
    }  
} 

void moveNext(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling moveNext() on NULL list");
        exit(EXIT_FAILURE);
    }

    //if cursor is defined and not at back
    if(L -> cursor != NULL && L -> cursor -> next != NULL) {
        L -> cursor = L -> cursor -> next;
        L -> index++;
    }

    //if curosr is defined and at back   
    else if(L -> cursor != NULL && L -> cursor -> next == NULL) {
        L -> cursor = NULL;
        L -> index = -1;
    }     
} 

void prepend(List L, int x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling prepend() on NULL list");
        exit(EXIT_FAILURE);
    }

    Node newElement = newNode(x);

    //if L is empty
    if(L -> length == 0) {
        L -> front = newElement;
        L -> back = newElement;
        newElement -> next = NULL;
        L -> length++;
    } else {
   	//if L is not empty, insert take place b4 front element
        newElement -> next = L -> front;
        L -> front -> prev = newElement;
        L -> front = newElement;
        L -> length++;
    }

    if(L -> index != -1) {
        L -> index++;
    }
}

void append(List L, int x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling append() on NULL list");
        exit(EXIT_FAILURE);
    }

    Node newElement = newNode(x);
    if(L -> length == 0) {
        L -> front = newElement;
        L -> back = newElement;
        newElement -> next = NULL;
        L -> length++;
    } else {
        newElement -> prev = L -> back;
        L -> back -> next = newElement;
        L -> back = newElement;
        L -> length++;
    }

}

void insertBefore(List L, int x) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling insertBefore() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling insertBefore() on empty list");
        exit(EXIT_FAILURE);
    }

    if(L -> index < 0) {
        fprintf(stderr, "List Error: calling inserBefore() on empty index");
        exit(EXIT_FAILURE);
    }

    if(L -> index == 0) {
        prepend(L, x);
    } else {
        Node newElement = newNode(x);

        //set prev pointer of newElement to point to element that comes b4 cursor
	newElement -> prev = L->cursor -> prev;

        // set the next pointer of newElment to point to cursor element in list
        newElement -> next = L -> cursor;

        //update next pointer of element that comes b4 cursor to new elmenet [1] -> [x]
        L -> cursor -> prev -> next = newElement;

        // update prev pointer of cursor to point to new elment [1] -> [x] -> [2] -> [3].
        L -> cursor -> prev = newElement;

	L -> length++;
	L -> index++;
    }
}

void insertAfter(List L, int x) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling insertBefore() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling insertBefore() on empty list");
        exit(EXIT_FAILURE);
    }

    if(L -> index < 0) {
        fprintf(stderr, "List Error: calling inserBefore() on empty index");
        exit(EXIT_FAILURE);
    }

    if(L -> cursor == L -> back) {
        append(L, x);
    } else {
        Node newElement = newNode(x);

        //set prev pointer of newElement to point to current  cursor
        newElement -> prev = L -> cursor;

        //set next pointer of newElement to point to the next element in the cursor list
        newElement -> next = L -> cursor -> next;

        //update prev pointer element that comes after cursor to point to new element 
        L -> cursor -> next -> prev = newElement;

        //update next pointer of cursor to point to new elmenet [x]. 
        L -> cursor -> next = newElement;

        L -> length++;
    }
}

void deleteFront(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling deleteFront() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling deleteFront() on empty list");
        exit(EXIT_FAILURE);
    }
   
    Node temp = L -> front; 
    if(L -> length == 1) {
        freeNode(&L -> front);
        L -> front = L -> back = L -> cursor = NULL;
        L -> length = 0;
        L -> index = -1;
    } else {
        L -> front = L -> front -> next;
	freeNode(&temp);
        L -> front -> prev = NULL;
        L -> index--;
        L -> length--;
    }

}

void deleteBack(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling deleteBack() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length < 1) {
        fprintf(stderr, "List Error: calling deleteBack() on empty list");
        exit(EXIT_FAILURE);
    }

    Node temp = L -> back;
    if(L -> length == 1) {
        freeNode(&L -> back);
        L -> front = L -> back = L -> cursor = NULL;
        L -> length = 0;
        L -> index = -1;
    } else {
	    if (L->cursor == L -> back) {
            	L->cursor = NULL; 
            	L->index = -1;
            }
        L -> back = L -> back -> prev;
	freeNode(&temp);
        L -> back -> next = NULL;
        L -> length--;
    }
}

void delete(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling delete() on NULL list");
        exit(EXIT_FAILURE);
    }

    if(L -> length <= 0) {
        fprintf(stderr, "List Error: calling delete() on empty list");
        exit(EXIT_FAILURE);
    }

    if(L->cursor == L -> front) {
	deleteFront(L);
   }
   else if(L -> cursor == L -> back) {
	deleteBack(L);
    } else {
	freeNode(&L -> cursor);
        L -> length--;
    }
}

/*********** Other Functions ***********/
void printList(FILE* out, List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling printList() on NULL list");
        exit(EXIT_FAILURE);
    }

    Node N = NULL;
    for(N = L -> front; N != NULL; N = N -> next) {
        fprintf(out, "%d ", N -> data);
    }
}

List copyList(List L) {
    if(L == NULL) {
        fprintf(stderr, "List Error: calling copyLIst() on NULL list");
        exit(EXIT_FAILURE);
    }
    
    List copiedList = newList();

    Node N = L -> front;
    while(N != NULL) {
        append(copiedList, N -> data);
        N = N -> next;
    }

    return copiedList;
}


