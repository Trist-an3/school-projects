#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>

#define TOTAL_BUFFER 4096

char buffer[TOTAL_BUFFER]; //buffer to hold ipnut data
ssize_t bytesWrittenGlobal; // global var to store # of bytes written
char *command; // command extracted from input
char *filePath; // file path extracted from input
char *additionalContent; //cool stuff extracted from input
int contentLength; //additional content length

//handles operation errors
void operationError() {
    fprintf(stderr, "Operation Failed\n");
    exit(1);
}

//handles command errors
void commandError() {
    fprintf(stderr, "Invalid Command\n");
    exit(1);
}

//handle "get" command
void getCommand(const char *filePath) {
    //open dah file
    int file_descriptor = open(filePath, O_RDONLY, 0);
    if (file_descriptor < 0) {
        commandError();
    }
    //read from file tand write to stdout
    while (1) {
        ssize_t bytesWritten = 0;
        ssize_t readStuff = read(file_descriptor, buffer, TOTAL_BUFFER);
        if (readStuff == 0)
            break;
        if (readStuff == -1) {
            operationError();
        }
        for (bytesWritten = 0; bytesWritten < readStuff; bytesWritten += bytesWrittenGlobal) {
            bytesWrittenGlobal
                = write(STDOUT_FILENO, buffer + bytesWritten, readStuff - bytesWritten);
            if (bytesWrittenGlobal == -1) {
                operationError();
            }
        }
    }
    //close file
    bytesWrittenGlobal = close(file_descriptor);
    if (bytesWrittenGlobal < 0) {
        operationError();
    }
}

//handle "set" command
void setCommand(const char *filePath, const char *additionalContent) {
    //open/create filexd
    int file_descriptor = open(filePath, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (file_descriptor < 0) {
        commandError();
    }
    //write additional extra crap to file
    ssize_t bytesread = 4 + strlen(filePath) + 1 + strlen(additionalContent) + 1;
    char *cont = buffer + bytesread;
    ssize_t bytesToWrite
        = (contentLength < (TOTAL_BUFFER - bytesread)) ? contentLength : (TOTAL_BUFFER - bytesread);

    ssize_t writeCount = 0;
    for (writeCount = 0; writeCount < bytesToWrite; writeCount += bytesWrittenGlobal) {
        bytesWrittenGlobal = write(file_descriptor, cont + writeCount, bytesToWrite - writeCount);
        if (bytesWrittenGlobal == 0) {
            break;
        }
        if (bytesWrittenGlobal == -1) {
            operationError();
        }
    }
    contentLength -= writeCount;
    //read additional stuff from stdin and write 2 file
    ssize_t byteslefttowrite = 0;
    do {
        ssize_t bytesWritten = 0;
        ssize_t readStuff = read(STDIN_FILENO, buffer, TOTAL_BUFFER);
        if (readStuff == 0)
            break;
        if (readStuff == -1) {
            commandError();
        }
        for (bytesWritten = 0; bytesWritten < readStuff; bytesWritten += bytesWrittenGlobal) {
            bytesWrittenGlobal
                = write(file_descriptor, buffer + bytesWritten, readStuff - bytesWritten);
            if (bytesWrittenGlobal == -1) {
                operationError();
            }
        }
        byteslefttowrite += bytesWritten;
    } while (byteslefttowrite < contentLength);

    //close fiel pogw
    bytesWrittenGlobal = close(file_descriptor);
    if (bytesWrittenGlobal < 0) {
        operationError();
    }
    //if successfulpogw
    ssize_t writeStatus = write(STDOUT_FILENO, "OK\n", 3);
    if (writeStatus < 0) {
        operationError();
    }
}

int main(void) {
    ssize_t total_bytes_read = 0;
    //read input from stdin into buffer
    do {
        ssize_t bytes_read
            = read(STDIN_FILENO, buffer + total_bytes_read, TOTAL_BUFFER - total_bytes_read);
        if (bytes_read == 0)
            break;
        if (bytes_read == -1) {
            operationError();
        }
        total_bytes_read += bytes_read;
    } while (total_bytes_read < TOTAL_BUFFER);
    //extract the command("set or get")
    command = strtok(buffer, "\n");
    //check command if valid
    if ((strcmp(command, "get") == 0) && (buffer[total_bytes_read - 1] != '\n')) {
        commandError();
    }
    //extract file path from bufer
    filePath = strtok(NULL, "\n");
    //check if file valid or whatevs
    if (filePath) {
        //check file path length
        if (strlen(filePath) > PATH_MAX) {
            commandError();
        }
        //execture the get command pog
        if (strcmp(command, "get") == 0) {
            //extract any extra info
            additionalContent = strtok(NULL, "");
            if (additionalContent) {
                commandError();
            }
            getCommand(filePath);
        } else if (strcmp(command, "set") == 0) {
            //extract any other stuff once set command is in play
            additionalContent = strtok(NULL, "\n");
            if (additionalContent != NULL) {
                contentLength = atoi(additionalContent);
                if ((strcmp(additionalContent, "0") != 0) && contentLength <= 0) {
                    commandError();
                }
            } else {
                commandError();
            }
            setCommand(filePath, additionalContent);
        } else {
            commandError();
        }

    } else {
        commandError();
    }

    return 0;
}
