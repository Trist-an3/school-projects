/********************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA4
* File: Matrix.c
* ADT file for Matrix functions
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "Matrix.h"

/*********** Structs ***********/
typedef struct EntryObj* Entry;

typedef struct EntryObj {
    int column;
    double data;
} EntryObj;


typedef struct MatrixObj {
    int size;
    int NNZ;
    List *entries;
} MatrixObj;

/*********** Constructors Destructors ***********/
Entry newEntry(int column, double data) {
    Entry E = malloc(sizeof(EntryObj));
    E -> column = column;
    E -> data = data;

    return E;
}

void freeEntry(Entry* pE) {
    if(pE != NULL && *pE != NULL) {
        free(*pE);
        *pE = NULL;
    }
}

Matrix newMatrix(int n) {
    Matrix M = malloc(sizeof(MatrixObj));
    M -> entries = calloc(n + 1, sizeof(List));

    for(int i = 1; i <= n; i++) {
        M -> entries[i] = newList();
    }

    M -> size = n;
    M -> NNZ = 0;

    return M;
}

void freeMatrix(Matrix* pM) {
    if(pM != NULL && *pM != NULL) {
        for(int i = 1; i <= (*pM)->size; i++) {
            moveFront((*pM) -> entries[i]);
            while(index((*pM) -> entries[i]) != -1) {
                Entry currentEntry = get((*pM) -> entries[i]);
                freeEntry(&currentEntry);
                moveNext((*pM) -> entries[i]);
            }
            freeList(&((*pM)->entries[i]));
        }

        free((*pM)->entries);
        free(*pM);
        *pM = NULL;
    }
}

/*********** Access Functions ***********/
int size(Matrix M){
    if (M == NULL) {
        fprintf(stderr, "NULL Error: calling size() on null matrix\n"); 
        exit(EXIT_FAILURE);
    }

    return M -> size;
}

int NNZ(Matrix M){
    if (M == NULL) {
        fprintf(stderr, "NULL Error: calling size() on null matrix\n");
        exit(EXIT_FAILURE);
    }

    return M -> NNZ;
}

int equals(Matrix A, Matrix B) {
    if (A == NULL || B == NULL) {
        //return 0;
	exit(EXIT_FAILURE);
    }


    if (size(A) != size(B)) {
        return 0;
    }

    for (int i = 1; i <= size(A); i++) {
        moveFront(A->entries[i]);
        moveFront(B->entries[i]);
        while (index(A->entries[i]) != -1 && index(B->entries[i]) != -1) {
            Entry EA = (Entry) get(A->entries[i]);
            Entry EB = (Entry) get(B->entries[i]);

            if (EA->data != EB->data || EA->column != EB->column) {
                return 0;
            }
            moveNext(A->entries[i]);
            moveNext(B->entries[i]);
        }
       if (index(A->entries[i]) != index(B->entries[i])) {
            return 0;
        }
    }
    return 1;
}

/*********** Manipulation procedures ***********/
void makeZero(Matrix M){
    if (M == NULL) {
        fprintf(stderr, "NULL Error: calling size() on null matrix\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i <= size(M); i++) {
	clear(M -> entries[i]);
    }
    
   // M -> size = 0;
    M -> NNZ = 0;
}

void changeEntry(Matrix M, int i, int j, double x){
    if(M == NULL) {
        fprintf(stderr, "NULL Error: calling changeEntry() on null matrix\n");
        exit(EXIT_FAILURE);
    }

   if (i < 1 || i > size(M) || j < 1 || j > size(M)) {
       return;
   }

    List L = M -> entries[i];

    if (x != 0) {
        if (length(L) == 0) {
            prepend(L, newEntry(j, x));
            M -> NNZ++;
        } else {
            for(moveFront(L); index(L) != -1 && ((Entry)get(L)) -> column < j; moveNext(L));
            if (index(L) == -1) {
                append(L, newEntry(j, x));
                M -> NNZ++;
            } else if (((Entry)get(L)) -> column == j) {
                ((Entry)get(L)) -> data = x;
            } else {
                insertBefore(L, newEntry(j, x));
                M -> NNZ++;
            }
        }
    } else {
        for(moveFront(L); index(L) != -1 && ((Entry)get(L)) -> column < j; moveNext(L));
       if (index(L) != -1 && ((Entry)get(L))->column == j) {
            Entry currentEntry;
            currentEntry = ((Entry)get(L));
            freeEntry(&currentEntry);
            delete(L);
            M -> NNZ--;
	    
        }
    }
}

/*********** Matrix Arithmetic operations ***********/
Matrix copy(Matrix A){
    if(A == NULL) {
        fprintf(stderr, "NULL Error: calling copy() on null matrix\n");
        exit(EXIT_FAILURE);
    }

    Matrix C = newMatrix(size(A));

    for(int i  = 1; i <= size(A); i++) {
        moveFront(A -> entries[i]);
        while(index(A -> entries[i]) != -1) {
            Entry currentEntry = ((Entry)get(A -> entries[i]));
            changeEntry(C, i, currentEntry -> column, currentEntry -> data);
            moveNext(A -> entries[i]);
        }
    }

    return C;
}

Matrix transpose(Matrix A){
    if(A == NULL) {
        fprintf(stderr, "NULL Error: calling transpose() on null matrix\n");
        exit(EXIT_FAILURE);
    }

    Matrix T = newMatrix(sizeof(A));

    for(int i  = 1; i <= size(A); i++) {
        moveFront(A -> entries[i]);
        while(index(A -> entries[i]) != -1) {
            Entry currentEntry = ((Entry)get(A -> entries[i]));
            changeEntry(T, currentEntry -> column, i, currentEntry -> data);
            moveNext(A -> entries[i]);
        }
    }

    return T;
}

Matrix scalarMult(double x, Matrix A){
    if(A == NULL) {
        fprintf(stderr, "NULL Error: calling scalarMult() on null matrix\n");
        exit(EXIT_FAILURE);
    }
    
    Matrix SM = newMatrix(size(A));

    for(int i = 1; i <= size(A); i++) {
        moveFront(A -> entries[i]);
        while(index(A -> entries[i]) != -1) {
            Entry currentEntry = ((Entry)get(A -> entries[i]));
            changeEntry(SM, i, currentEntry -> column, currentEntry -> data * x);
            moveNext(A -> entries[i]);
        }
    }

    SM -> NNZ = A -> NNZ;
    return SM;
}

Matrix sum(Matrix A, Matrix B){
    if (A == NULL || B == NULL) {
        fprintf(stderr, "Null Error: calling sum() on a NULL Matrix.");
        exit(EXIT_FAILURE);
    }

    if(size(A) != size(B)) {
        fprintf(stderr, "NULL Error: calling sum() on unequal size.\n");
        exit(EXIT_FAILURE);
    }

    if(equals(A, B)) {
        return scalarMult(2, A);
    } else { 
        Matrix Add = newMatrix(size(A));
        for (int i = 1; i <= Add -> size; i++) {
            moveFront(A->entries[i]);
            moveFront(B->entries[i]);
            while (index(A->entries[i]) != -1 && index(B->entries[i]) != -1) {
                Entry currentEntryA = (Entry)get(A->entries[i]);
                Entry currentEntryB = (Entry)get(B->entries[i]);
                if (currentEntryA->column < currentEntryB->column) {
                    append(Add -> entries[i], newEntry(currentEntryA->column, currentEntryA->data));
                    Add -> NNZ++;
                    moveNext(A->entries[i]);
                }
                else if (currentEntryA->column > currentEntryB->column) {
                    append(Add -> entries[i], newEntry(currentEntryB->column, currentEntryB->data));
                    Add -> NNZ++;
                    moveNext(B->entries[i]);
                }
                else {
                    if (currentEntryA->data + currentEntryB->data != 0) {
                        append(Add -> entries[i], newEntry(currentEntryA->column, currentEntryA->data + currentEntryB->data));
                        Add -> NNZ++;
                    }
                    moveNext(A->entries[i]);
                    moveNext(B->entries[i]);
                }
            }
            while (index(A->entries[i]) != -1) {
                Entry currentEntryA = (Entry)get(A->entries[i]);
                append(Add -> entries[i], newEntry(currentEntryA->column, currentEntryA->data));
                Add -> NNZ++;
                moveNext(A->entries[i]);
            }
            while (index(B->entries[i]) != -1) { 
                Entry currentEntryB = (Entry)get(B->entries[i]);
                append(Add -> entries[i], newEntry(currentEntryB->column, currentEntryB->data));
                Add -> NNZ++;
                moveNext(B->entries[i]);
            }
        }
        return Add;
    }
} 

Matrix diff(Matrix A, Matrix B){
    if (A == NULL || B == NULL) {
        fprintf(stderr, "Null Error: calling sum() on a NULL Matrix.");
        exit(EXIT_FAILURE);
    }

    if(size(A) != size(B)) {
        fprintf(stderr, "NULL Error: calling sum() on unequal size.\n");
        exit(EXIT_FAILURE);
    }

    Matrix Diff = newMatrix(size(A));

    for (int i = 1; i <= Diff -> size; i++) {
        moveFront(A->entries[i]);
        moveFront(B->entries[i]);
        while (index(A->entries[i]) != -1 && index(B->entries[i]) != -1) {
            Entry currentEntryA = (Entry)get(A->entries[i]);
            Entry currentEntryB = (Entry)get(B->entries[i]);
            if (currentEntryA -> column < currentEntryB -> column) {
                append(Diff -> entries[i], newEntry(currentEntryA -> column, currentEntryA -> data));
                Diff->NNZ++;
                moveNext(A -> entries[i]);
            } else if (currentEntryA -> column > currentEntryB -> column) {
                append(Diff -> entries[i], newEntry(currentEntryB -> column, -1 * currentEntryB -> data));
                Diff->NNZ++;
                moveNext(B -> entries[i]);
            } else {
                double difference = currentEntryA -> data - currentEntryB -> data;
                if (difference != 0) {
                    append(Diff->entries[i], newEntry(currentEntryA -> column, difference));
                    Diff->NNZ++;
                }
                moveNext(A -> entries[i]);
                moveNext(B -> entries[i]);
            }
        }
        while (index(A -> entries[i]) != -1) {
            Entry currentEntryA = (Entry)get(A -> entries[i]);
            append(Diff->entries[i], newEntry(currentEntryA -> column, currentEntryA -> data));
            Diff->NNZ++;
            moveNext(A -> entries[i]);
        }

        while (index(B -> entries[i]) != -1) {
            Entry currentEntryB = (Entry)get(B -> entries[i]);
            append(Diff->entries[i], newEntry(currentEntryB -> column, -1 * currentEntryB -> data));
            Diff->NNZ++;
            moveNext(B -> entries[i]);
        }
    }

    return Diff;
}


double vectorDot(List P, List Q) {
    if (P == NULL || Q == NULL) {
        printf("Debug: P or Q is NULL\n");
        return 0.0;
    }
    double dotProduct = 0.0;

    moveFront(P);
    moveFront(Q);

    while (index(P) != -1 && index(Q) != -1) {
        Entry currentEntryP = 	(Entry)get(P);
        Entry currentEntryQ = (Entry)get(Q);
        if (currentEntryP -> column == currentEntryQ -> column) {
            dotProduct += currentEntryP -> data * currentEntryQ -> data;
            moveNext(P);
            moveNext(Q);
        } else if (currentEntryP -> column < currentEntryQ -> column) {
            moveNext(P);
	    continue;
        } else if (currentEntryP -> column > currentEntryQ -> column)  {
            moveNext(Q);
            continue;
        }
    }
    return dotProduct;
}

Matrix product(Matrix A, Matrix B) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "Matrix Errro: Calling product() on one or more NULL Matricies.");
        exit(EXIT_FAILURE);
    }
   if (A->size != B->size) {
        return NULL;
    }

    Matrix P = newMatrix(A->size);
    if (A->NNZ == 0 || B->NNZ == 0) {
        return P;
    }
    List pL;
    Matrix BT = transpose(B);
    for (int i = 1; i <= A->size; i++) {
        pL = P->entries[i];
        if (length(A->entries[i]) > 0) {
            for (int j = 1; j <= BT->size; j++) {
                if (length(BT->entries[j]) > 0) {
                    double dp = vectorDot(A->entries[i], BT->entries[j]);
                    if (dp != 0) {
                        append(pL, newEntry(j, dp));
                        P->NNZ++;        
                    }
                }
            }
        }
    }
    freeMatrix(&BT);
    return P;
}

/*********** Other Functions ***********/
/*
void printMatrix(FILE* out, Matrix M) {
    for (int i = 1; i <= size(M); i++) {
        moveFront(M->entries[i]);
        if(length(M->entries[i]) <= 0) {
            continue;
        }
        fprintf(out, "%d: ", i);
        while (index(M->entries[i]) >= 0) {
            Entry N = get(M->entries[i]);
            fprintf(out, "(%d, %.1f) ", N->column, N->data);
            moveNext(M->entries[i]);
        }
        fprintf(out, "\n");
    }
}
*/
void printMatrix(FILE* out, Matrix M) {
    if (M == NULL) {
        fprintf(stderr, "Error: Matrix is NULL\n");
        return;
    }

    for (int i = 1; i <= size(M); i++) {
        List row = M->entries[i];
        if (row == NULL) {
            fprintf(stderr, "Error: List at index %d is NULL\n", i);
            continue;
        }

        moveFront(row);
        if (length(row) <= 0) {
            continue;
        }

        fprintf(out, "%d: ", i);
        for (moveFront(row); index(row) >= 0; moveNext(row)) {
            Entry entry = (Entry) get(row);
            fprintf(out, "(%d, %.1f) ", entry->column, entry->data);
        }
        fprintf(out, "\n");
    }
}

