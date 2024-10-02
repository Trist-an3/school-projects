/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA4
* File: List.h
* Header file for the List ADT with function prototypes
*********************************************************/

#pragma once

#include <stdbool.h>

typedef struct ListObj* List;

typedef void* ListElement;

/*********** Constructors Destructors ***********/
// Creates and returns a new empty List.
List newList(void);

// Frees all heap memory associated with *pL, and set to NULL
void freeList(List* pL); 

/*********** Access Functions ***********/
int length(List L); 

// Returns index of cursor element if defined, -1 otherwise.
int index(List L);  

// Returns front element of L. Pre: length()>0
ListElement front(List L);  

// Returns back element of L. Pre: length()>0
ListElement back(List L);   

// Returns cursor element of L. Pre: length()>0, index()>=0
ListElement get(List L);    

/*********** Manipulation procedures ***********/
// Resets L to its original empty state.
void clear(List L);      

// Overwrites the cursor element’s data with x. 
// Pre: length()>0, index()>=0
void set(List L, ListElement x);

// If L is non-empty, sets cursor under the front element, otherwise does nothing.
void moveFront(List L);  

// If L is non-empty, sets cursor under the back element, otherwise does nothing.
void moveBack(List L);  

// If cursor is defined and not at front, move cursor one step toward the front of L
// If cursor is defined and at front, cursor becomes undefined; if cursor is undefined
// do nothing
void movePrev(List L); 

// If cursor is defined and not at back, move cursor one step toward the back of L
// If cursor is defined and at back, cursor becomes undefined; if cursor is undefined
// do nothing
void moveNext(List L); 

// Insert new element into L. If L is non-empty, insertion takes place before front element.
void prepend(List L, ListElement x); 

// Insert new element into L. If L is non-empty, insertion takes place after back element.
void append(List L, ListElement x); 

// Insert new element before cursor.
// Pre: length()>0, index()>=0
void insertBefore(List L, ListElement x); 

// Insert new element after cursor.
// Pre: length()>0, index()>=0
void insertAfter(List L, ListElement x); 

// Delete the front element. Pre: length()>0
void deleteFront(List L);

// Delete the back element. Pre: length()>0
void deleteBack(List L); 

// Delete cursor element, making cursor undefined.
// Pre: length()>0, index(void delete(List L);    
void delete(List L);

