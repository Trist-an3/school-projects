#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>

#include "asgn2_helper_funcs.h"

#define BUFSIZE 4096
#define REQEX   "^([a-zA-Z]{1,8}) /([a-zA-Z0-9.-]{1,63}) (HTTP/[0-9]\\.[0-9])\r\n"
#define HEADEX  "([a-zA-Z0-9.-]{1,128}): ([ -~]{1,128})\r\n"

//represents http msg containing request or response info
typedef struct HttpMessage {
    char *method; // HTTP method
    char *path; // target path or resource URI.
    char *http_version;
    char *body;
    int client_fd; //file descriptor
    int content_length;
    int remaining_bytes;
} HttpMessage;

//Sends an HTTP status response to the client socket descriptor based on the status code
void send_status(int code, int sd, int needok, long int contlen) {
    char message[BUFSIZE] = { 0 }; //buffer to hold http response msg
    switch (code) {
    case 200:
        //the HTTP response for "OK" status
        snprintf(
            message, sizeof(message), "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", contlen);
        // Send the response header
        send(sd, message, strlen(message), 0);
        // If needok flag is set, send "OK" message
        if (needok == 1) {
            send(sd, "OK\n", 3, 0);
        }
        break;
    case 201:
        //created response
        send(sd, "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n", 51, 0);
        break;
    case 400:
        //bad request response
        send(sd, "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n", 60, 0);
        break;
        //forbidden response
    case 403:
        send(sd, "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n", 56, 0);
        break;
        //not found response
    case 404:
        send(sd, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n", 56, 0);
        break;
        //internal server error
    case 500:
        send(sd,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 23\r\n\r\nInternal Server "
            "Error\n",
            80, 0);
        break;
        //not implemented
    case 501:
        send(sd, "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n", 68,
            0);
        break;
        //version not supported
    case 505:
        send(sd,
            "HTTP/1.1 505 Version Not Supported\r\nContent-Length: 22\r\n\r\nVersion Not "
            "Supported\n",
            80, 0);
        break;
    }
}

//handle get request
int get(HttpMessage *R) {
    // Check if the content length is set (should be -1 for GET requests)
    if (R->content_length != -1 || R->remaining_bytes > 0) {
        send_status(400, R->client_fd, 0, 12); // Bad Request
        return EXIT_FAILURE;
    }

    // Open the requested file
    int fd = open(R->path, O_RDONLY);
    if (fd == -1) {
        // Handle file not found or access denied errors
        if (errno == ENOENT) {
            send_status(404, R->client_fd, 0, 10); // Not Found
        } else if (errno == EACCES) {
            send_status(403, R->client_fd, 0, 10); // Forbidden
        } else {
            send_status(500, R->client_fd, 0, 22); // Internal Server Error
        }
        return EXIT_FAILURE;
    }

    // Check if the requested path is a directory
    struct stat path_stat;
    if (fstat(fd, &path_stat) == -1 || S_ISDIR(path_stat.st_mode)) {
        close(fd);
        send_status(403, R->client_fd, 0, 10); // Forbidden
        return EXIT_FAILURE;
    }

    // Retrieve file size and send HTTP response header with content length
    off_t size = path_stat.st_size;
    dprintf(R->client_fd, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", size);

    // Send file contents to the client
    int bytes_written = pass_n_bytes(fd, R->client_fd, size);
    if (bytes_written == -1) {
        send_status(500, R->client_fd, 0, 22); // Internal Server Error
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS; ///////pog
}

int put(HttpMessage *R) {
    // Check if the content length is provided (required for PUT requests)
    if (R->content_length == -1) {
        send_status(400, R->client_fd, 0, 12); // Bad Request
        return EXIT_FAILURE;
    }

    // Open the file with write-only mode, creating if not exists, failing if it's a directory
    int fd = open(R->path, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd == -1) {
        // Handle errors
        if (errno == EEXIST) {
            // File already exists, open it with truncation
            fd = open(R->path, O_WRONLY | O_TRUNC);
            if (fd == -1) {
                send_status(403, R->client_fd, 0, 10); // Forbidden
                return EXIT_FAILURE;
            }
        } else if (errno == EACCES) {
            send_status(403, R->client_fd, 0, 10); // Forbidden
            return EXIT_FAILURE;
        } else {
            send_status(500, R->client_fd, 0, 22); // Internal Server Error
            return EXIT_FAILURE;
        }
    }

    // Write the request body to the file
    int bytes_written = write_n_bytes(fd, R->body, R->remaining_bytes);
    if (bytes_written == -1) {
        send_status(500, R->client_fd, 0, 22); // Internal Server Error
        close(fd);
        return EXIT_FAILURE;
    }

    // If content length is greater than remaining bytes, pass the rest from client to file
    int total_written = R->content_length - R->remaining_bytes;
    bytes_written = pass_n_bytes(R->client_fd, fd, total_written);
    if (bytes_written == -1) {
        send_status(500, R->client_fd, 0, 22); // Internal Server Error
        close(fd);
        return EXIT_FAILURE;
    }

    // Send the appropriate status code based on whether the file was created or overwritten
    if (errno == EEXIST) {
        dprintf(R->client_fd, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nOK\n", 3);
    } else {
        send_status(201, R->client_fd, 0, 8); // Created
    }

    close(fd);
    return EXIT_SUCCESS;
}

//handle HTTP requests
int handle_request(HttpMessage *R) {
    // Check if the provided HTTP version is valid (must be HTTP/1.1)
    if (strncmp(R->http_version, "HTTP/1.1", 8) != 0) {
        send_status(505, R->client_fd, 0, 22); //version not supported vibes
        return EXIT_FAILURE;
        // Check if the HTTP method is GET
    } else if (strncmp(R->method, "GET", 3) == 0) {
        return get(R);
        // Check if the HTTP method is PUT
    } else if (strncmp(R->method, "PUT", 3) == 0) {
        return put(R);
        // If the HTTP method is neither get or put, send Not Implemented code
    } else {
        send_status(501, R->client_fd, 0, 16); // not implemented
        return EXIT_FAILURE;
    }
}

int parse_request(HttpMessage *R, char *buf, ssize_t bytes_read) {
    int total_offset = 0; // Total offset used for parsing
    regex_t re; // Regular expression object for pattern matching
    regmatch_t matches[4]; // Array to store matched substrings
    int rc; // Return code from regex functions

    // Compile and execute regular expression to match the HTTP request line
    rc = regcomp(&re, REQEX, REG_EXTENDED);
    if (rc != 0) {
        send_status(500, R->client_fd, 0, 22); // Internal Server Error
        return EXIT_FAILURE;
    }

    rc = regexec(&re, buf, 4, matches, 0);
    if (rc != 0) {
        send_status(400, R->client_fd, 0, 12); // Bad Request
        regfree(&re);
        return EXIT_FAILURE;
    }

    // If the request line matches the expected format
    R->method = buf;
    R->path = buf + matches[2].rm_so;
    R->http_version = buf + matches[3].rm_so;

    buf[matches[1].rm_eo] = '\0';
    R->path[matches[2].rm_eo - matches[2].rm_so] = '\0';
    R->http_version[matches[3].rm_eo - matches[3].rm_so] = '\0';

    buf += matches[3].rm_eo + 2;
    total_offset += matches[3].rm_eo + 2;

    regfree(&re);

    // Initialize content length to -1
    R->content_length = -1;

    // Compile and execute regular expression to match header lines
    rc = regcomp(&re, HEADEX, REG_EXTENDED);
    if (rc != 0) {
        send_status(500, R->client_fd, 0, 22); // Internal Server Error
        return EXIT_FAILURE;
    }

    rc = regexec(&re, buf, 3, matches, 0);
    while (rc == 0) {
        buf[matches[1].rm_eo] = '\0';
        buf[matches[2].rm_eo] = '\0';

        // Check if the header line is Content-Length and extract its value
        if (strncmp(buf, "Content-Length", 14) == 0) {
            int value = strtol(buf + matches[2].rm_so, NULL, 10);
            if (errno == EINVAL) {
                send_status(400, R->client_fd, 0, 12); // Bad Request
                regfree(&re);
                return EXIT_FAILURE;
            }
            R->content_length = value;
        }

        //move buffer pointer and upate total offset
        buf += matches[2].rm_eo + 2;
        total_offset += matches[2].rm_eo + 2;

        //execture regex again fo find header line
        rc = regexec(&re, buf, 3, matches, 0);
    }

    // Check if the end of headers is reached and extract message body if present
    if ((rc != 0) && (buf[0] == '\r' && buf[1] == '\n')) {
        R->body = buf + 2;
        total_offset += 2;
        R->remaining_bytes = bytes_read - total_offset;
    } else if (rc != 0) {
        send_status(400, R->client_fd, 0, 12); // Bad Request
        regfree(&re);
        return EXIT_FAILURE;
    }

    regfree(&re);
    return EXIT_SUCCESS;
}

//main
int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    char buf[BUFSIZE + 1] = { '\0' }; // Buffer to store incoming HTTP request messages

    Listener_Socket socket; // Listener socket for accepting incoming connections
    int port = strtol(argv[1], NULL, 10); //Convert the port number from string to integer

    // Check for conversion errors
    if (errno == EINVAL) {
        fprintf(stderr, "Invalid Port\n"); //error msg
        return EXIT_FAILURE;
    }

    // Initialize the listener socket with the provided port number
    int socket_status = listener_init(&socket, port);

    // Check if the listener socket initialization fails
    if (socket_status == -1) {
        fprintf(stderr, "Invalid Port\n");
        return EXIT_FAILURE;
    }

    // Continuous loop to accept incoming connections and handle HTTP requests
    while (true) {
        // Accept a new client connection and get the socket file descriptor
        int sock_fd = listener_accept(&socket);

        // Check if the client connection establishment fails
        if (sock_fd == -1) {
            fprintf(stderr, "Cant establish connection\n");
            return EXIT_FAILURE;
        }

        // Create an HttpMessage object to store information about the HTTP request
        HttpMessage R;
        R.client_fd = sock_fd; // Set the client socket file descriptor

        // Read the incoming HTTP request message into the buffer
        ssize_t bytes_read = read_until(sock_fd, buf, BUFSIZE, "\r\n\r\n");

        // Check if there is an error while reading the request message
        if (bytes_read == -1) {
            // Send a Bad Request response to the client and exit with failure
            dprintf(R.client_fd,
                "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
            return EXIT_FAILURE;
        }

        // Parse the HTTP request message and handle the request if parsing is successful
        if (parse_request(&R, buf, bytes_read) != EXIT_FAILURE) {
            handle_request(&R);
        }

        close(sock_fd);
        memset(buf, '\0', sizeof(buf));
    }
    return EXIT_SUCCESS;
}
