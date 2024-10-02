/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA6
* File: List.cpp
* ADT file for the List Functions
*******************************************************/

#include <iostream> //standard library
#include <string>
#include <stdexcept> //error handling

#include "List.h"

using namespace std;

/*********** Private Constructors ***********/
List::Node::Node(ListElement x) {
   data = x;
   next = nullptr;
   prev = nullptr;   
}

/*********** Constructors & Destructors ***********/
List::List() {
   frontDummy = new Node(-500);
   backDummy = new Node(-500);
   frontDummy -> next= backDummy;
   backDummy -> prev = frontDummy;
   beforeCursor = frontDummy;
   afterCursor = backDummy;
   pos_cursor = 0;
   num_elements = 0;
}

List::List(const List& L) {
   frontDummy = new Node(-500);
   backDummy = new Node(-500);
   frontDummy -> next = backDummy;
   backDummy -> prev = frontDummy;
   beforeCursor = frontDummy;
   afterCursor = backDummy;
   pos_cursor = 0;
   num_elements = 0;

   Node *N = L.frontDummy -> next;
   while(N != L.backDummy) {
      this -> insertBefore(N -> data);
      N = N -> next;
   }
}

List::~List() {
   clear();
   delete frontDummy;
   delete backDummy;
}


/*********** Access Functions ***********/
int List::length() const {
   return num_elements;
}

ListElement List::front() const {
   if(num_elements <= 0) {
      throw std::length_error("List Error: calling front() on empty list.");
   }

   return frontDummy -> next -> data;
}

ListElement List::back() const {
   if(num_elements <= 0) {
      throw std::length_error("List Error: calling back() on empty list.");
   }

   return backDummy -> prev -> data;
}

int List::position() const {
   return pos_cursor;
}

ListElement List::peekNext() const {
   if(position() >= length()) {
      throw std::range_error("List Error: calling peekNext() on invalid range");
   }

   return afterCursor -> data;
}

ListElement List::peekPrev() const {
   if(position() <= 0) {
      throw std::range_error("List Error: calling peekPrev() on invalid range");
   }

   return beforeCursor -> data;
}

/*********** Manipulation procedures ***********/
void List::clear() {
    Node* current = frontDummy->next;  
    while (current != backDummy) {
        Node* temp = current; 
        current = current->next;  
        delete temp; 
    }

    frontDummy->next = backDummy;
    backDummy->prev = frontDummy;
    beforeCursor = frontDummy;
    afterCursor = backDummy;
    pos_cursor = 0;
    num_elements = 0;
}

void List::moveFront() {
   beforeCursor = frontDummy;
   afterCursor = frontDummy -> next;
   pos_cursor = 0;
}

void List::moveBack() {
   afterCursor = backDummy;
   beforeCursor = backDummy -> prev;
   pos_cursor = num_elements;
}

ListElement List::moveNext() {
   if(position() >= length()) {
      throw std::range_error("List Error: calling moveNext() on invalid range");
   }

   beforeCursor = afterCursor;
   afterCursor = afterCursor -> next;
   pos_cursor++;

   return beforeCursor -> data;
}

ListElement List::movePrev() {
   if(position() <= 0) {
      throw std::range_error("List Error: calling movePrev() on invalid range");
   }

   afterCursor = beforeCursor;
   beforeCursor = beforeCursor -> prev;
   pos_cursor--;

   return afterCursor -> data;
}

void List::insertAfter(ListElement x) {
   Node* newNode = new Node(x);
   newNode -> next = afterCursor;
   newNode -> prev = beforeCursor;
   afterCursor -> prev = newNode;
   beforeCursor -> next = newNode;
   afterCursor = newNode;
   num_elements++;
}

void List::insertBefore(ListElement x) {
   Node* newNode = new Node(x);
   newNode -> next = afterCursor;
   newNode -> prev = beforeCursor;
   afterCursor -> prev = newNode;
   beforeCursor -> next = newNode;
   beforeCursor = newNode;
   pos_cursor++;
   num_elements++;
}

void List::setAfter(ListElement x) {
   if(position() >= length()) {
      throw std::range_error("List Error: calling setAfter() on invalid range");
   }

   afterCursor -> data = x;
}

void List::setBefore(ListElement x) {
   if(position() <= 0) {
      throw std::range_error("List Error: calling setBefore() on invalid range");
   }

   beforeCursor -> data = x;
}

void List::eraseAfter() {
   if(position() >= length()) {
      throw std::range_error("List Error: calling eraseAfter() on invalid range");
   }

   Node* nodeToDelete = afterCursor;
   afterCursor = nodeToDelete->next;
   beforeCursor->next = afterCursor;
   afterCursor->prev = beforeCursor;

   delete nodeToDelete;
   num_elements--;
   

}

void List::eraseBefore() {
   if(position() <= 0) {
      throw std::range_error("List Error: calling eraseBefore() on invalid range");
   }

   Node * nodeToDelete = beforeCursor;
   beforeCursor = nodeToDelete->prev;
   afterCursor->prev = beforeCursor;
   beforeCursor->next = afterCursor;

   delete nodeToDelete;
   num_elements--;
   pos_cursor--;
}

/*********** Other Functions ***********/
int List::findNext(ListElement x) {
   while(afterCursor != backDummy) {
      if(afterCursor -> data == x) {
         moveNext();
	 return pos_cursor;
      }
      moveNext();
   }
   return -1;

}

 int List::findPrev(ListElement x) {

   while(beforeCursor != frontDummy) {
      if(beforeCursor -> data == x) {
         movePrev();
	 return pos_cursor;
      }
      movePrev();
   }
      return -1;
}

void List::cleanup(){
    int uniqueElementCount = 0;
    int currentCursorPos = pos_cursor;  
    Node* currentNode = frontDummy->next;

    do {
       moveFront();
       int nextFindResult = findNext(currentNode->data);
       while(nextFindResult != -1){

            nextFindResult = findNext(currentNode->data);
            if(nextFindResult == -1){
                break;
            }
            if(pos_cursor <= currentCursorPos){
                currentCursorPos--;
            }
            eraseBefore();
        }
        currentNode = frontDummy->next;
        uniqueElementCount++;
        int j = 0;
        while(j < uniqueElementCount){
            currentNode = currentNode->next;
            j++;
        }
    } while(currentNode != backDummy);
    
    moveFront();
    int j = 0;
    while(j < currentCursorPos){
        moveNext();
        j++;
    }
}



List List::concat(const List& L) const {
   List result;
   Node* N = this -> frontDummy -> next;
   Node* M = L.frontDummy -> next;
   while(N != this -> backDummy) {
      result.insertBefore(N -> data);
      N = N -> next;
   }

   while(M != L.backDummy) {
      result.insertBefore(M -> data);
      M = M -> next;
   }
   result.moveFront();
   return result;
}

std::string List::to_string() const {
   Node* N = nullptr;
   std::string s = "(";

   for(N = frontDummy -> next; N != backDummy; N = N -> next) {
         s += std::to_string(N -> data);
	 if(N -> next != backDummy) {
	    s+= ", ";
	 }
   }
   
   s += ")";
   return s;
}

bool List::equals(const List& R) const {
   bool eq = false;
   Node* N = nullptr;
   Node* M = nullptr;

   eq = (this -> num_elements == R.num_elements);
   N = this -> frontDummy -> next;
   M = R.frontDummy -> next;
   while(eq && N != backDummy) {
      eq = (N -> data == M -> data);
      N = N -> next;
      M = M -> next;
   }
   return eq;
}

/*********** Overriden Operators ***********/
std::ostream& operator<<( std::ostream& stream, const List& L ) {
   return stream << L.List::to_string();
}

bool operator==( const List& A, const List& B ) {
   return A.List::equals(B);
}

List& List::operator=(const List& L) {
   if(this != &L) {
      List temp = L;

      std::swap(frontDummy, temp.frontDummy);
      std::swap(backDummy, temp.backDummy);
      std::swap(beforeCursor, temp.beforeCursor);
      std::swap(afterCursor, temp.afterCursor);
      std::swap(pos_cursor, temp.pos_cursor);
      std::swap(num_elements, temp.num_elements);
   }

   return *this;
}
