#include "hangman_helpers.h"

bool is_lowercase_letter(char c) {
    if ('a' <= c && c <= 'z') {
        return true;
    } else {
        return false;
    }
}

bool validate_secret(const char *secret) {
    if (strlen(secret) > MAX_LENGTH) {
        printf(CLEAR_SCREEN);
        return false;
    }

    for (int i = 0; secret[i] != '\0'; i++) {
        char c = secret[i]; //this gets the invlaid character lawl

        if (!is_lowercase_letter(c) && c != ' ' && c != '-' && c != '\'') {
            printf("invalid character: '%c'\nthe secret phrase must contain only "
                   "lowercase letters, spaces, hyphens, and apostrophes\n",
                c);
            return false;
        }
    }
    return true;
}

bool string_contains_character(const char *s, char c) {

    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == c) {
            return true;
            break;
        }
    }

    return false;
}

char read_letter(void) {

    int c;
    printf("Guess a letter: ");
    c = getchar();

    while (getchar() != '\n') {
        continue;
    }

    return (char) c;
}
