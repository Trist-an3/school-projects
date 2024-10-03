/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA8
* File: Dictionary.cpp
* ADT file for the Dictionary functions
********************************************************/

#include <string>
#include <iostream>
#include <stdexcept>
#include "Dictionary.h"

#define BLACK 0
#define RED 1

using namespace std;

/*********** Private Constructors ***********/
Dictionary::Node::Node(keyType k, valType v) {
    key = k;
    val = v;
    color = BLACK;
    parent = nullptr;
    left = nullptr;
    right = nullptr;
}
/*********** Constructors & Destructors ***********/
Dictionary::Dictionary() {
    nil = new Node("\0", -420);
    root = nil;
    current = nil;
    num_pairs = 0;
}

Dictionary::Dictionary(const Dictionary& D) {
    nil = new Node("\0", -420);
    root = nil;
    current = nil;
    num_pairs = 0;
    preOrderCopy(D.root, D.nil);
}

Dictionary::~Dictionary() {
    clear();
    delete nil;
}

/*********** Helper Functions ***********/
void Dictionary::inOrderString(std::string& s, Node* R) const {
    if(R != nil) {
        inOrderString(s, R -> left);
        s += R -> key + " : " + std::to_string(R -> val) + "\n";
        inOrderString(s, R -> right);
    }
}

void Dictionary::preOrderString(std::string& s, Node* R) const {
    if(R != nil) {
        s += R -> key;
        if (R->color == RED){
            s += " (RED)";
        }
        s += "\n";
        preOrderString(s, R -> left);
        preOrderString(s, R -> right);
    }
}

void Dictionary::preOrderCopy(Node* R, Node* N) {
    if(R != N) {
        setValue(R -> key, R -> val);
        preOrderCopy(R -> left, N);
        preOrderCopy(R -> right, N);
    }
}

void Dictionary::postOrderDelete(Node* R) {
    if(R != nil) {
        postOrderDelete(R -> left);
        postOrderDelete(R -> right);
        delete R;
    }
}

/*
void Dictionary::transplant(Node *u, Node* v) {
    if(u -> parent == nil) {
        root = v;
    }
    else if(u == u -> parent -> left) {
        u -> parent -> left = v;
    } else {
        u -> parent -> right = v;
    }
    
    if(v != nil) {
        v -> parent = u -> parent;
    }
}
*/

Dictionary::Node* Dictionary::search(Node* R, keyType k) const {
    if(R == nil || k == R -> key) {
        return R;
    }
    else if(k < R -> key) {
        return search(R -> left, k);
    }
    else {
        return search(R -> right, k);
    }
}



Dictionary::Node* Dictionary::findMin(Node* R) {
    if(R == nil) {
        return nil;
    }

    while(R -> left != nil) {
        R = R -> left;
    }

    return R;
}

Dictionary::Node* Dictionary::findMax(Node* R) {
    if(R == nil) {
        return nil;
    }
    
    while(R -> right != nil) {
        R = R -> right;
    }

    return R;
}

Dictionary::Node* Dictionary::findNext(Node* N) {
    if(N -> right != nil) {
        return findMin(N -> right);
    }

    Node* Y = N -> parent;
    while(Y != nil && N == Y -> right) {
        N = Y;
        Y = Y -> parent;
    }

    return Y;
}

Dictionary::Node* Dictionary::findPrev(Node* N) {
    if(N -> left != nil) {
        return findMax(N -> left);
    }

    Node* Y = N -> parent;
    while(Y != nil && N == Y -> left) {
        N = Y;
        Y = Y -> parent;
    }

    return Y;
}

/*********** RBT Helper Functions ***********/   
void Dictionary::LeftRotate(Node* N) {
    Node* Y = N -> right;

    N -> right = Y -> left;
    if(Y -> left != nil) {
        Y -> left -> parent = N;
    }

    Y -> parent = N -> parent;
    if(N -> parent == nil) {
        root = Y;
    }
    else if(N == N -> parent -> left) {
        N -> parent -> left = Y;
    } else {
        N -> parent -> right = Y;
    }

    Y -> left = N;
    N -> parent = Y;
}

void Dictionary::RightRotate(Node* N) {
    Node* Y = N -> left;

    N -> left = Y -> right;
    if(Y -> right != nil) {
        Y -> right -> parent = N;
    }

    Y -> parent = N -> parent;
    if(N -> parent == nil) {
        root = Y;
    }
    else if(N == N -> parent -> right) {
        N -> parent -> right = Y;
    } else {
        N -> parent -> left = Y;
    }

    Y -> right = N;
    N -> parent = Y;
}

void Dictionary::RB_InsertFixUp(Node* N) {
    while(N -> parent -> color == RED) {
        if(N -> parent == N -> parent -> parent -> left) {
            Node* Y = N -> parent -> parent -> right;
            if(Y -> color == RED) {
                N -> parent -> color = BLACK;
                Y -> color = BLACK;
                N -> parent -> parent -> color = RED;
                N = N -> parent -> parent;
            } else {
                if(N == N -> parent -> right) {
                    N = N -> parent;
                    LeftRotate(N);
                }
                N -> parent -> color = BLACK;
                N -> parent -> parent -> color = RED;
                RightRotate(N -> parent -> parent);
            }
        } else {
            Node* Y = N -> parent -> parent -> left;
            if(Y -> color == RED) {
                N -> parent -> color = BLACK;
                Y -> color = BLACK;
                N -> parent -> parent -> color = RED;
                N = N -> parent -> parent;
            } else {
                if(N == N -> parent -> left) {
                    N = N -> parent;
                    RightRotate(N);
                }
                N -> parent -> color = BLACK;
                N -> parent -> parent -> color = RED;
                LeftRotate(N -> parent -> parent);
            }
        }
    }
    root -> color = BLACK;
}

void Dictionary::RB_Transplant(Node* u, Node* v) {
    if(u -> parent == nil) {
        root = v;
    }
    else if(u == u -> parent -> left) {
        u -> parent -> left = v;
    } else {
        u -> parent -> right = v;
    }
    v -> parent = u -> parent;
}

void Dictionary::RB_DeleteFixUp(Node* N) {
    while(N != root && N -> color == BLACK) {
        if(N == N -> parent -> left) {
            Node* W = N -> parent -> right;
            if(W -> color == RED) {
                W -> color = BLACK;
                N -> parent -> color = RED;
                LeftRotate(N -> parent);
                W = N -> parent -> right;
            }
            if(W -> left -> color == BLACK && W -> right -> color == BLACK) {
                W -> color = RED;
                N = N -> parent;
            } else {
                if(W -> right -> color == BLACK) {
                    W -> left -> color = BLACK;
                    W -> color = RED;
                    RightRotate(W);
                    W = N -> parent -> right;
                }
                    
                W -> color = N -> parent -> color;
                N -> parent -> color = BLACK;
                W -> right -> color = BLACK;
                LeftRotate(N -> parent);
                N = root;
            }
        } else {
            Node* W = N -> parent -> left;
            if(W -> color == RED) {
                W -> color = BLACK;
                N -> parent -> color = RED;
                RightRotate(N -> parent);
                W = N -> parent -> left;
            }
            if(W -> right -> color == BLACK && W -> left -> color == BLACK) {
                W -> color = RED;
                N = N -> parent;
            } else {
                if(W -> left -> color == BLACK) {
                    W -> right -> color = BLACK;
                    W -> color = RED;
                    LeftRotate(W);
                    W = N -> parent -> left;
                }
                W -> color = N -> parent -> color;
                N -> parent -> color = BLACK;
                W -> left -> color = BLACK;
                RightRotate(N -> parent);
                N = root;
            }
        }
    }
    N -> color = BLACK;
}

void Dictionary::RB_Delete(Node* N) {
    Node* Y = N;
    Node* X;
    int Y_original_color = Y -> color;
    
    if(N -> left == nil) {
        X = N -> right;
        RB_Transplant(N, N -> right);
    }
    else if(N -> right == nil) {
        X = N -> left;
        RB_Transplant(N, N -> left);
    } else {
        Y = findMin(N -> right);
        Y_original_color = Y -> color;
        X = Y -> right;
        if(Y -> parent == N) {
            X -> parent = Y;
        } else {
            RB_Transplant(Y, Y -> right);
            Y -> right = N -> right;
            Y -> right -> parent = Y;
        }
        RB_Transplant(N, Y);
        Y -> left = N -> left;
        Y -> left -> parent = Y;
        Y -> color = N -> color;
    }
    if(Y_original_color == BLACK) {
        RB_DeleteFixUp(X);
    }

}

/*********** Access Functions ***********/
int Dictionary::size() const { 
    return num_pairs;
}

bool Dictionary::contains(keyType k) const {
    Node* N = search(root, k);
    if(N != nil) {
        return true;
    } else {
        return false;
    }
}


valType& Dictionary::getValue(keyType k) const {
    if(!contains(k)) {
        throw std::logic_error("Dictionary: getValue(): key doesn't exist");
    }

    Node* N = search(root, k);
    return N -> val;
}


bool Dictionary::hasCurrent() const {
    if(current != nil) {
        return true;
    } else {
        return false;
    }
}

keyType Dictionary::currentKey() const {
    if(!hasCurrent()) {
        throw std::logic_error("Dictionary: currentKey(): current doesn't exist");
    }

    return current -> key;
}


valType& Dictionary::currentVal() const {
    if(!hasCurrent()) {
        throw std::logic_error("Dictionary: currentVal(): current doesn't exsit");
    }

    return current -> val;
}

/*********** Manipulation procedures ***********/
void Dictionary::clear() {
    postOrderDelete(root);
    root = nil;
    current = nil;
    num_pairs = 0;
}

void Dictionary::setValue(keyType k, valType v) {
    Node* Z = new Node(k, v);
    //Z -> left = nil;
   // Z -> right = nil;
    //Z -> color = RED;

    Node* Y = nil;
    Node* X = root;
    while(X != nil) {
        Y = X;
        if(Z -> key < X -> key) {
            X = X -> left;
        } 
        else if(Z -> key > X -> key) {
            X = X -> right;
        } else {
            X -> val = v;
	    delete Z;
            return;
        }
    }

    Z -> parent = Y;
    if(Y == nil) {
        root = Z;
    }
    else if(Z -> key < Y -> key) {
        Y -> left = Z;
    } else {
        Y -> right = Z;
    }
    
    Z -> left = nil;
    Z -> right = nil;
    Z -> color = RED;
    RB_InsertFixUp(Z);
    num_pairs++;
}

void Dictionary::remove(keyType k) {
    if(!contains(k)) {
        throw std::logic_error("Dictionary: remove(): key doesn't exist");
    }

    Node* Z = search(root, k);
    if(Z == current) {
        current = nil;    
    }

    RB_Delete(Z);
    delete Z;
    num_pairs--;
}

void Dictionary::begin() {
    if(num_pairs > 0) {
        current = findMin(root);
    }
}

void Dictionary::end() {
    if(num_pairs > 0) {
        current = findMax(root);
    }
}

void Dictionary::next() {
    if(!hasCurrent()) {
       // throw std::logic_error("Dictionary: next(): current is not defined");
      return;
    }

    Node *N = findNext(current);
    if(N == nil) {
        current = nil;
    }
    else if(N != nil) {
        current = N;
    }
}


void Dictionary::prev() {
    if(!hasCurrent()) {
        //throw std::logic_error("Dictionary: prev(): current is not defined");
        return;
    }
    Node *N = findPrev(current);
    if(N == nil) {
        current = nil;
    }
    else if(N!= nil) {
        current = N;
    }
}

/*********** Other Functions ***********/
std::string Dictionary::to_string() const {
    std::string s = "";
    inOrderString(s, root);
    return s;
}

std::string Dictionary::pre_string() const {
    std::string s = "";
    preOrderString(s, root);
    return s;
}

bool Dictionary::equals(const Dictionary& D) const {
    std::string D1, D2;
    D1 = to_string();
    D2 = D.to_string();

    if((num_pairs == D.num_pairs) && (D1 == D2)) {
        return true;
    } else {
        return false;
    }
}

/*********** Overload Operators ***********/
std::ostream& operator<<( std::ostream& stream, Dictionary& D ) {
        return stream << D.to_string();
}

bool operator==( const Dictionary& A, const Dictionary& B ) {
    return A.equals(B);
}

Dictionary& Dictionary::operator=( const Dictionary& D ) {
    if(this != &D) {
        Dictionary temp = D;

        std::swap(nil, temp.nil);
        std::swap(root, temp.root);
        std::swap(current, temp.current);
        std::swap(num_pairs, temp.num_pairs);
    }

    return *this;
}
