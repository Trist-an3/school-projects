#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguements\n");
        fprintf(stderr, "usage: %s: [<delimiter> <file1> <file2> ...] <split_char> \n", argv[0]);
        return 1;
    }

    //check if no delimiter is passed
    if (strlen(argv[1]) == 0) {
        fprintf(stderr, "No delimiter specified\n");
        return 1;
    }

    //check if delimiter is more than one character
    if (strlen(argv[1]) > 1) {
        fprintf(stderr, "Delimiter must be a single character\n");
        return 1;
    }

    char delimiter = argv[1][0];

    //check if delimiter is a newline
    if (delimiter == '\n') {
        fprintf(stderr, "Delimiter cannot be a newline character\n");
        return 1;
    }

    if (delimiter == '\0') {
        fprintf(stderr, "Delimiter cannot be a null character\n");
        return 1;
    }

    // if (delimiter == ' ') {
    //     fprintf(stderr, "Delimiter cannot be a null character\n");
    //     return 1;
    // }

    //char *delimiter = argv[1];
    for (int i = 2; i < argc; i++) {
        int fd;
        if (strcmp(argv[i], "-") == 0) {
            fd = STDIN_FILENO;
        }
        //test
        else if (strcmp(argv[i], "..") == 0) {
            return 1;

        } else {
            fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "%s: missing: No such file or directory\n", argv[0]);
                //return 1;
                continue;
            }
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(fd, &buffer, BUFFER_SIZE)) > 0) {
            for (ssize_t j = 0; j < bytesRead; j++) {
                if (buffer[j] == delimiter) {
                    buffer[j] = '\n';
                }
            }

            ssize_t bytesWritten = write(STDOUT_FILENO, buffer, bytesRead);
            if (bytesWritten < 0) {
                fprintf(stderr, "Error writing to stdout\n");
                return 1;
            }
        }

        if (fd != 0) {
            //write(STDOUT_FILENO, "\n", 1);
            close(fd);
        }
    }

    return 0;
}
