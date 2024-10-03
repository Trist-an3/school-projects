/*Tristan Garcia
 * CSE13s
 * Program is a virtual version of Pass the Pigs players have to 
 * roll the pig and get awarded x amount of points and game finishes
 * when player reaches 100 points
 */

#include "names.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_PLAYERS   2
#define DEFAULT_SEED  2023
#define WINNING_SCORE 100

//an enum data type that is named Position and holds 5 differ positions
typedef enum { SIDE, RAZORBACK, TROTTER, SNOUTER, JOWLER } Position;

//const array named pig of the type Position and stores values from the enum
const Position pig[7] = {
    SIDE,
    SIDE,
    RAZORBACK,
    TROTTER,
    SNOUTER,
    JOWLER,
    JOWLER,
};

int main(void) {
    //variable initialization
    int current_player = 0;
    int num_players = 0;
    int seed = 0;
    int dice_roll = 0;
    int skip_turn = false;

    printf("Number of players (2 to 10)? ");
    scanf("%d", &num_players);

    //conditional check for # of players
    if (num_players < MIN_PLAYERS || num_players > 10) {
        fprintf(stderr, "Invalid number of players. Using 2 instead.\n");
        num_players = MIN_PLAYERS;
    }

    //conditional check for seed value
    printf("Random-number seed? ");
    if (scanf("%d", &seed) != 1) {
        fprintf(stderr, "Invalid seed. Using 2023 instead.\n");
        seed = DEFAULT_SEED;
        srandom(seed);
    } else {
        srandom(seed);
    }

    //initialized player scores for each player in the game
    int player_score[num_players];

    //initializes all player scores to 0
    for (int i = 0; i < num_players; i++) {
        player_score[i] = 0;
    }
    printf("%s\n", current_player[player_name]);

    //core mechanics for game
    while (current_player[player_score] < WINNING_SCORE) {

        //if player rolls side then this block gets triggered
        if (skip_turn) {
            skip_turn = false;
            current_player = (current_player + 1) % num_players;
            printf("%s\n", current_player[player_name]);
            continue;
        }

        //simulates a random dice roll and determines a dice value
        dice_roll = pig[random() % 7];
        int dice_value = 0;

        switch (dice_roll) {
        case SIDE: skip_turn = true; break;

        case RAZORBACK: dice_value = 10; break;

        case TROTTER: dice_value = 10; break;

        case SNOUTER: dice_value = 15; break;

        case JOWLER: dice_value = 5; break;
        }

        //update current players score based on dice roll
        current_player[player_score] += dice_value;
        printf(" rolls %d, has %d\n", dice_value, current_player[player_score]);

        //check if the current player has won
        if (current_player[player_score] >= 100) {

            printf("%s won!\n", current_player[player_name]);
            break;
        }
    }

    return 0;
}
