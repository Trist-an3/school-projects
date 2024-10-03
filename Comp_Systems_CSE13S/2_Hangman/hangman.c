/*hangman game :^)
 * Tristan Garcia
 * CSE13s
 * hangman game that handles user input and validation*/
#include "hangman_helpers.h"

int main(int argc, char *argv[]) {
    unsigned long phrase_length = 0;

    char guess = '\0';
    char secret[MAX_LENGTH] = { 0 };
    char display_secret[phrase_length];
    char eliminated_guess[LOSING_MISTAKE] = { 0 };
    char correct_guess[phrase_length];

    int gallows_state = 0;
    int mistakes_made = 0;
    bool game_over = false;
    bool valid_guess = false;

    //user must input a second arguement in the command line
    if (argc < 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Usage: %s <secret word or phrase>\n", argv[0]);
        fprintf(stderr, "If the secret is multiple words, you must quote it\n");
        game_over = true;
    }

    for (int i = 1; i < argc; i++) {
        strcat(secret, argv[i]);
        phrase_length = strlen(secret);

        if (i < argc - 1) {
            strcat(secret, " "); // Add a space between words
        }
    }

    if (!validate_secret(secret)) {
        fprintf(stderr, "");
        exit(1);
    }

    //core mechanics
    while (!game_over) {

        valid_guess = false;

        // Set up the initial display of the secret phrase with underscores for hidden letters,
        // and reveal special characters like hyphens or apostrophes.
        for (unsigned long i = 0; i < phrase_length; i++) {
            if (secret[i] == '-' || secret[i] == '\'' || secret[i] == ' '
                || secret[i] == correct_guess[i]) {
                display_secret[i] = secret[i];
            } else {
                display_secret[i] = '_';
            }
        }

        printf(CLEAR_SCREEN);
        printf("%s\n\n", arts[gallows_state]);
        printf("    Phrase: %s\n", display_secret);
        printf("Eliminated: %s\n\n", eliminated_guess);

        //has to be valid input or it loops
        while (!valid_guess) {
            guess = read_letter();
            if (is_lowercase_letter(guess) && !string_contains_character(correct_guess, guess)
                && !string_contains_character(eliminated_guess, guess)) {
                valid_guess = true;
            }
        }

        //checks the string for guesses and collects inputs
        if (string_contains_character(secret, guess)) {
            for (unsigned long i = 0; i < phrase_length; i++) {
                if (secret[i] == guess) {
                    correct_guess[i] = guess;
                    display_secret[i] = guess;
                }
            }
        } else {
            if (!string_contains_character(eliminated_guess, guess)) {
                int i = 0;
                while (i < mistakes_made && guess > eliminated_guess[i]) {
                    i++;
                }
                for (int j = mistakes_made; j > i; j--) {
                    eliminated_guess[j] = eliminated_guess[j - 1];
                }

                eliminated_guess[i] = guess;
                mistakes_made++;
            }

            //updates stickman
            if (mistakes_made < 7) {
                gallows_state = mistakes_made;
            }
        }

        //if user wins game
        if (strcmp(display_secret, secret) == 0) {
            printf(CLEAR_SCREEN);
            printf("%s\n\n", arts[gallows_state]);
            printf("    Phrase: %s\n", display_secret);
            printf("Eliminated: %s\n\n", eliminated_guess);
            printf("You win! The secret phrase was: %s\n", display_secret);
            game_over = true;
        }

        //if user loses game
        if (mistakes_made >= LOSING_MISTAKE) {
            printf(CLEAR_SCREEN);
            printf("%s\n\n", arts[gallows_state]);
            printf("    Phrase: %s\n", display_secret);
            printf("Eliminated: %s\n\n", eliminated_guess);
            printf("You lose! The secret phrase was: %s\n", secret);
            game_over = true;
        }
    }

    return 0;
}
