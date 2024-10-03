/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA5
* File: Shuffle.cpp
* The main file
********************************************************/

#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "List.h"

using namespace std;

void shuffle(List& D) {
    List A;
    List B;
    int deckSize = D.length();
    int splitA = deckSize / 2;

    D.moveFront();
    if (D.position() < splitA) {
        do {
            A.insertBefore(D.moveNext());
        } while (D.position() < splitA);
    }
    if (D.position() < deckSize) {
        do {
            B.insertBefore(D.moveNext());
        } while (D.position() < deckSize);
    }

    D.clear();
    A.moveFront();
    B.moveFront();
    if (B.position() < B.length()) {
        do {
            D.insertBefore(B.moveNext());
            if (A.position() < A.length()) {
                D.insertBefore(A.moveNext());
            }
        } while (B.position() < B.length());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw invalid_argument("Invalid Usage: Correct way is ./Shuffle <integer>");
    }

    int deck_size = atoi(argv[1]);

    cout << "deck size       shuffle count" << endl;
    cout << "------------------------------" << endl;
    for (int n = 1; n <= deck_size; ++n) {
        List Deck;
        for(int i = 1; i <= n; i++) {
            Deck.insertBefore(i);
        }
        int shuffle_count = 0;
        int max_shuffles = 1000;
        List original_order = Deck;
        do {
            shuffle(Deck);
            shuffle_count++;
        } while (!Deck.equals(original_order) && shuffle_count < max_shuffles);

        cout << " " << left << setw(16) << n << shuffle_count << endl;

    }

    return 0;
}
