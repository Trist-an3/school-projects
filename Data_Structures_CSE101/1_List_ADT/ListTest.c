#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"List.h"

int main(int argc, char* argv[]){
   
   List A = newList();
   List B = newList();
   List C = NULL;
   int i;

   //testing append and prepend
   for(i=1; i<=20; i++){
      append(A, i);
      prepend(B, i);
   }

   // Testing printList
   printf("List A: ");
   printList(stdout, A); 
   printf("\nList B: ");
   printList(stdout, B); 
   printf("\n");

   //testing Front and Back
   moveFront(A);
   printf("Front of A: %d\n", front(A));
   moveBack(B);
   printf("Back of B: %d\n", back(B));

   //testing set 
   moveFront(A);
   moveNext(A);
   set(A, 100);
   printf("List A after setting second element to 100: ");
   printList(stdout, A);
   set(A, 2);
   printf("\n");

   //testing deleteFront
   deleteFront(A);
   printf("List A after deleting front element: ");
   printList(stdout, A);
   printf("\n");

   // Testing deleteBack
   deleteBack(B);
   printf("List B after deleting back element: ");
   printList(stdout, B);
   printf("\n");
 
   //Testing index and get
   //Testing iteration through List A from front to back using moveFront
   printf("List A after iterating from front to back: ");
   for(moveFront(A); index(A)>=0; moveNext(A)){
      printf("%d ", get(A));
   }
   printf("\n");

   //testing iteration through List B from back to front using moveBack
   printf("List B after iterating from Back to Front: ");
   for(moveBack(B); index(B)>=0; movePrev(B)){
      printf("%d ", get(B));
   }
   printf("\n");

   //Test for copying list
   C = copyList(A);
   printf("After copying List A to List C: ");
   printList(stdout, C);
   printf("\n");
   
   //test for Equals function
   //First 2 should print false and last one should print true
   //Testing equality between List A and List B. Prints "true" if equal, "false" otherwise.
   printf("%s\n", equals(A,B)?"true":"false");

   //Testing equality between List B and List C. Prints "true" if equal, "false" otherwise.
   printf("%s\n", equals(B,C)?"true":"false");
   
   //Testing equality between List C and List A. Prints "true" if equal, "false" otherwise. 
   printf("%s\n", equals(C,A)?"true":"false");

   //Testing insertBefore and insertAfter,clear, delete, and FreeList
   moveFront(A);
   for(i=0; i<5; i++) moveNext(A); // at index 5
   insertBefore(A, -1);            // at index 6
   for(i=0; i<9; i++) moveNext(A); // at index 15
   insertAfter(A, -2);
   for(i=0; i<5; i++) movePrev(A); // at index 10

   //testing delete function
   delete(A);
   printList(stdout,A);
   printf("\n");
   printf("%d\n", length(A));

   ///testing clear function
   clear(A);
   printf("%d\n", length(A));

   //Testing FreeList function
   freeList(&A);
   freeList(&B);
   freeList(&C);

   return 0;
}

/*
 * Output of program
 * Tested all functions
 * Reason why bottom one is 20 and not 21 is b/c i deleted some of the nodes while testing
 *
 * List A: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * List B: 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 
 * Front of A: 1
 * Back of B: 1
 * List A after setting second element to 100: 1 100 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * List A after deleting front element: 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * List B after deleting back element: 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 
 * List A after iterating from front to back: 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * List B after iterating from Back to Front: 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * After copying List A to List C: 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
 * false
 * false
 * true
 * 2 3 4 5 6 -1 7 8 9 10 12 13 14 15 16 -2 17 18 19 20 
 * 20
 * 0
 */

 
