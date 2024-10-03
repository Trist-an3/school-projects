#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <regex.h>
#include "asgn2_helper_funcs.h"
#include "queue.h"
#include "rwlock.h"
#include "debug.h"
#include "protocol.h"

#define BUFF_EXTRA       256
#define MAX_REQUEST_SIZE 2048

#define METHOD_CHUNK_LENGTH       9
#define URI_CHUNK_LENGTH          65
#define HTTP_VERSION_CHUNK_LENGTH 11
#define HEADERS_CHUNK_LENGTH      0

#define _METHOD_PATTERN       "^([a-zA-Z]{1,8}) "
#define _URI_PATTERN          "^/([a-zA-Z0-9\\.-]{1,63}) "
#define _HTTP_VERSION_PATTERN "^HTTP/([0-9])\\.([0-9])\r\n"
#define _HEADERS_PATTERN      "^([a-zA-Z0-9\\.-]{1,128}: [ -~]{1,128}\r\n)*\r\n"
#define _HEADER_PATTERN       "([a-zA-Z0-9\\.-]{1,128}): ([ -~]{1,128})\r\n"

//parsing a chunk of data
#define parse_and_check_chunk(req, chunk_size, pattern, n_groups)                                  \
    regmatch_t matches[n_groups];                                                                  \
    const int res = validate_and_extract_chunk(req, chunk_size, pattern, matches, n_groups);       \
    if (res != 0) {                                                                                \
        return res;                                                                                \
    }

//initialize a regex pattern
#define INIT_REGEX(reg)                                                                            \
    if (regcomp(&_regs[regexInitialized], _##reg##_PATTERN, REG_EXTENDED | REG_NEWLINE) != 0) {    \
        clear_http_regex();                                                                        \
        fprintf(stderr, "Failed to initialize %s regex pattern (%s)\n", #reg, _##reg##_PATTERN);   \
        return -1;                                                                                 \
    } else {                                                                                       \
        _##reg##_REG = &_regs[regexInitialized++];                                                 \
    }

//create unsent server response
#define CREATE_UNSENT_RESPONSE(code)                                                               \
    (ServerResponse) {                                                                             \
        false, code                                                                                \
    }

//create sent server response
#define CREATE_SENT_RESPONSE(code)                                                                 \
    (ServerResponse) {                                                                             \
        true, code                                                                                 \
    }

typedef int Buffer_Size;

typedef enum {
    HTTP_GET, //represents http get req method
    HTTP_PUT, //represents http put req method
    HTTP_UNSUPPORTED, //represents unsupported http req method
} RequestType;

typedef struct {
    char *HeaderKey; //key header in an HTTP req
    char *HeaderValue; //value header in http req
} KeyValue;

//forward declaration of req struct
typedef struct request Request;

typedef struct server_response {
    bool has_responded; //flag showing if response been sent
    int response_status; //http stats code of response
} ServerResponse;

//buffer for storing data with read and write positions
typedef struct {
    char buf[MAX_REQUEST_SIZE + BUFF_EXTRA]; //array to store data with a size of maxreqsize
    Buffer_Size read_position; //position to read data from buffer
    Buffer_Size write_position; //position to write data into buffer
} InputBuffer;

//http req with various components
struct request {
    InputBuffer input_buffer; //input buffer to store req data
    int sockfd; //file descriptor for request
    RequestType request_method; //type of http req method
    char *request_uri; //uri of req
    char http_version_major; //major vers of http protocol
    char http_version_minor; //minor vers of http protocol
    int number_of_headers; //number of headers in req
    KeyValue *request_headers; //array of key value pair representing req headers
    Buffer_Size request_body_size; //size of req body
    char *request_body; //content of req body
};

//file lock with info about lock
struct file_lock {
    rwlock_t *lock; //read write lock for file access
    char *filename; //name of file being accessed
    int users; //number of users accessing the file
};

static struct file_lock *file_locks; // Array of file locks used for managing file access

static regex_t *_METHOD_REG; // Regular expression for parsing HTTP method
static regex_t *_URI_REG; // Regular expression for parsing URI
static regex_t *_HTTP_VERSION_REG; // Regular expression for parsing HTTP version
static regex_t *_HEADER_REG; // Regular expression for parsing individual header
static regex_t *_HEADERS_REG; // Regular expression for parsing multiple headers
static regex_t _regs[5]; // Array to hold compiled regex patterns
static int regexInitialized = 0; // Counter to track initialized regex patterns

static Listener_Socket sock; // Socket for listening to incoming connections
static pthread_t *threads_arr; // Array of worker threads
static int thread_count; // Number of worker threads
static volatile bool running = true; // Flag to control the main loop execution
static pthread_mutex_t file_locks_mutex
    = PTHREAD_MUTEX_INITIALIZER; // Mutex for file locks synchronization

// create a new http req
Request *http_request_create(const int sockfd) {
    // Allocate memory for the Request obj
    Request *req = malloc(sizeof(Request));

    // Initialize the Request object with default values
    *req = (Request) {
        .input_buffer
        = { .read_position = 0, .write_position = 0 }, // Initialize input buffer positions
        .sockfd = sockfd, // Set the file descriptor
        .request_method = HTTP_UNSUPPORTED, // Set request method to unsupported initially
        .request_uri = NULL, // Initialize request URI to NULL
        .http_version_major = '0', // Initialize major version of HTTP protocol
        .http_version_minor = '0', // Initialize minor version of HTTP protocol
        .number_of_headers = 0, // Initialize number of headers to 0
        .request_headers = NULL, // Initialize request headers to NULL
        .request_body_size = 0, // Initialize request body size to 0
        .request_body = NULL // Initialize request body to NULL
    };

    return req; // Return the created Request object
}

void http_request_close(Request *req) {
    // Receive any remaining data into the input buffer
    ssize_t rb = recv(
        req->sockfd, req->input_buffer.buf + req->input_buffer.write_position, BUFF_EXTRA, 0);
    if (rb == -1) {
        perror("Error receiving data");
    }

    // Close the socket file descriptor
    if (close(req->sockfd) == -1) {
        perror("Error closing socket");
    }
}

//free memory allocated for an HTTP request object
void http_request_free(Request *req) {
    // Check if the request object is NULL
    if (req == NULL) {
        return; // Return if the request object is NULL
    }

    // Free the memory allocated for the request URI if it's not NULL
    if (req->request_uri != NULL) {
        free(req->request_uri); // Free the memory for the request URI
        req->request_uri = NULL; // Set the request URI pointer to NULL
    }

    // Free the memory allocated for each request header key and value, and then free the headers array
    if (req->request_headers != NULL) {
        int i = 0;
        while (i < req->number_of_headers) {
            if (req->request_headers[i].HeaderKey != NULL) {
                free(req->request_headers[i].HeaderKey); // Free the memory for the header key
                req->request_headers[i].HeaderKey = NULL; // Set the header key pointer to NULL
            }
            if (req->request_headers[i].HeaderValue != NULL) {
                free(req->request_headers[i].HeaderValue); // Free the memory for the header value
                req->request_headers[i].HeaderValue = NULL; // Set the header value pointer to NULL
            }
            i++;
        }
        free(req->request_headers); // Free the memory for the headers array
        req->request_headers = NULL; // Set the headers array pointer to NULL
    }

    // Free the memory allocated for the request object itself
    free(req); // Free the memory for the request object
}

// parse chunk of dataaa from input buffer using regex pattern
int validate_and_extract_chunk(Request *req, const Buffer_Size chunk_size, regex_t *reg,
    regmatch_t *matches, const size_t n_matches) {
    // Calculate the current size of data in the input buffer
    const Buffer_Size current_size
        = req->input_buffer.write_position - req->input_buffer.read_position;

    // Check if there is enough space in the buffer to read the chunk
    if (current_size < chunk_size && req->input_buffer.write_position < MAX_REQUEST_SIZE) {
        // Calculate remaining space to read
        Buffer_Size remaining_space = chunk_size - current_size;
        if (req->input_buffer.write_position + remaining_space > MAX_REQUEST_SIZE) {
            remaining_space = MAX_REQUEST_SIZE - req->input_buffer.write_position;
        }
        //read data into buffer
        const ssize_t bytes_read = read_n_bytes(
            req->sockfd, req->input_buffer.buf + req->input_buffer.write_position, remaining_space);
        if (bytes_read <= 0) {
            return -1; // Handle read errors
        } else {
            req->input_buffer.write_position += bytes_read;
        }
    }

    // Dynamically allocate memory for chunk_str based on the actual size needed
    char *chunk_str = malloc(
        (req->input_buffer.write_position - req->input_buffer.read_position + 1) * sizeof(char));
    if (chunk_str == NULL) {
        return -1; // Handle memory allocation failure
    }

    strncpy(chunk_str, req->input_buffer.buf + req->input_buffer.read_position,
        req->input_buffer.write_position - req->input_buffer.read_position);
    chunk_str[req->input_buffer.write_position - req->input_buffer.read_position] = '\0';

    const int regex_result = regexec(reg, chunk_str, n_matches, matches, 0);

    free(chunk_str); // Free dynamically allocated memory

    if (regex_result > 0) {
        return -1; // Handle regex execution failure
    }

    return 0;
}

// parse the HTTP request method from the input buffer
int parseRequestMethod(Request *req) {
    // Parse and check the method chunk using the defined METHOD_CHUNK_LENGTH and _METHOD_REG regex pattern
    parse_and_check_chunk(req, METHOD_CHUNK_LENGTH, _METHOD_REG, 2);

    // Calculate the length of the parsed method string
    const Buffer_Size method_len = matches[1].rm_eo - matches[1].rm_so;
    char method_str[method_len + 1]; // Create a buffer to store the method string

    // Copy the method string from the input buffer to the method_str buffer
    strncpy(method_str, req->input_buffer.buf + req->input_buffer.read_position + matches[1].rm_so,
        method_len);
    method_str[method_len] = '\0'; // Null-terminate the method string

    // Compare the parsed method string with known HTTP methods
    switch (method_str[0]) {
    case 'G':
        if (strcmp(method_str, "GET") == 0) {
            req->request_method = HTTP_GET; // Set request method to HTTP_GET if method is GET
        } else {
            req->request_method
                = HTTP_UNSUPPORTED; // Set request method to HTTP_UNSUPPORTED for unsupported methods
            fprintf(stderr, "Unsupported HTTP method: %s\n", method_str); // Print error message
            return -1; // Indicate an error
        }
        break;
    case 'P':
        if (strcmp(method_str, "PUT") == 0) {
            req->request_method = HTTP_PUT; // Set request method to HTTP_PUT if method is PUT
        } else {
            req->request_method
                = HTTP_UNSUPPORTED; // Set request method to HTTP_UNSUPPORTED for unsupported methods
            fprintf(stderr, "Unsupported HTTP method: %s\n", method_str); // Print error message
            return -1; // Indicate an error
        }
        break;
    default:
        req->request_method
            = HTTP_UNSUPPORTED; // Set request method to HTTP_UNSUPPORTED for unknown methods
        fprintf(stderr, "Unsupported HTTP method: %s\n", method_str); // Print error message
        return -1; // Indicate an error
    }

    req->input_buffer.read_position
        += matches[0].rm_eo; // Update the read position in the input buffer
    return 0; // Return success
}

//parse the URI from the request object
int parseRequestURI(Request *req) {
    // Parse and check the URI chunk using the defined URI regex pattern
    parse_and_check_chunk(req, URI_CHUNK_LENGTH, _URI_REG, 2);

    // Check if the URI match is unsuccessful
    if (matches[1].rm_so == -1 || matches[1].rm_eo == -1) {
        fprintf(stderr, "Failed to parse URI\n");
        return -1; // Indicate failure
    }

    // Calculate the length of the URI
    const Buffer_Size uri_len = matches[1].rm_eo - matches[1].rm_so;
    char *uri = malloc(uri_len + 1); // Allocate memory for the URI string

    // Check if memory allocation for the URI string failed
    if (uri == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1; // Indicate failure
    }

    // Copy the URI string from the input buffer to the allocated memory
    strncpy(
        uri, req->input_buffer.buf + req->input_buffer.read_position + matches[1].rm_so, uri_len);
    uri[uri_len] = '\0'; // Null-terminate the URI string

    // Free the previous URI if it exists
    if (req->request_uri != NULL) {
        free(req->request_uri);
    }

    req->request_uri = uri; // Set the request URI to the parsed URI
    req->input_buffer.read_position += matches[0].rm_eo; // Update the read position

    return 0; // Return success
}

//parse the HTTP version from the input buffer
int parse_http_protocol_version(Request *req) {
    // Parse and check the HTTP version chunk using the defined HTTP_VERSION_CHUNK_LENGTH and _HTTP_VERSION_REG regex pattern
    parse_and_check_chunk(req, HTTP_VERSION_CHUNK_LENGTH, _HTTP_VERSION_REG, 3);

    // Check if the regex match was successful
    if (matches[0].rm_so == -1 || matches[1].rm_so == -1 || matches[2].rm_so == -1) {
        fprintf(stderr, "Failed to parse HTTP version\n");
        return -1; // Indicate failure
    }

    // Extract the major and minor versions from the input buffer
    req->http_version_major
        = req->input_buffer.buf[req->input_buffer.read_position + matches[1].rm_so];
    req->http_version_minor
        = req->input_buffer.buf[req->input_buffer.read_position + matches[2].rm_so];

    // Update the read position in the input buffer
    req->input_buffer.read_position += matches[0].rm_eo;

    return 0; // Return success
}

// Function to parse and validate headers from the input buffer of the request
int parse_and_validate_headers(Request *req) {
    // Parse and check the headers chunk using the defined HEADERS_CHUNK_LENGTH and _HEADERS_REG regex pattern
    parse_and_check_chunk(req, HEADERS_CHUNK_LENGTH, _HEADERS_REG, 2);

    // Check if there are no headers to parse
    if (matches[1].rm_so == matches[1].rm_eo) {
        req->input_buffer.read_position += matches[0].rm_eo;
        return 0; // Return success
    }

    int number_of_headers = 0;
    KeyValue *request_headers = malloc(sizeof(KeyValue)); // Start with initial allocation

    const regoff_t headers_end = req->input_buffer.read_position + matches[1].rm_eo;
    do {
        regmatch_t header_match[3];
        const int reg_res = regexec(_HEADER_REG,
            req->input_buffer.buf + req->input_buffer.read_position, 3, header_match, 0);

        if (reg_res != 0) {
            free(request_headers);
            return -1; // Handle regex execution failure
        }

        const Buffer_Size header_key_length = header_match[1].rm_eo - header_match[1].rm_so;
        const Buffer_Size header_value_length = header_match[2].rm_eo - header_match[2].rm_so;

        // Reallocate memory for the headers array and populate the key and value
        request_headers = realloc(request_headers, (number_of_headers + 1) * sizeof(KeyValue));
        KeyValue *header = &request_headers[number_of_headers];
        header->HeaderKey = strndup(
            req->input_buffer.buf + req->input_buffer.read_position + header_match[1].rm_so,
            header_key_length);
        header->HeaderValue = strndup(
            req->input_buffer.buf + req->input_buffer.read_position + header_match[2].rm_so,
            header_value_length);

        // Update the read position in the input buffer after parsing a header
        req->input_buffer.read_position += header_match[0].rm_eo;
        number_of_headers++; // Increment the count of parsed headers

    } while (req->input_buffer.read_position
             < headers_end); // Continue parsing headers until the end of headers chunk

    req->number_of_headers
        = number_of_headers; // Set the total number of parsed headers in the request object
    req->request_headers
        = request_headers; // Assign the array of parsed headers to the request object
    req->input_buffer.read_position += 2; // Move the read position past the headers chunk delimiter

    return 0; // Return success
}

int process_request_body(Request *req) {
    const Buffer_Size cur_size = req->input_buffer.write_position - req->input_buffer.read_position;

    // Update the size of the request body if there is data available in the input buffer
    if (cur_size > 0) {
        req->request_body_size = cur_size;

        // Check if there is enough data in the buffer to assign to request_body
        if (req->input_buffer.read_position + cur_size <= MAX_REQUEST_SIZE) {
            req->request_body = req->input_buffer.buf + req->input_buffer.read_position;
        } else {
            // Handle the case where there is not enough space for the request body
            fprintf(stderr, "Not enough space in the buffer for the request body\n");
            return -1;
        }
    }

    //reset read position to write position to indicate buffer empty
    req->input_buffer.read_position = req->input_buffer.write_position;
    return 0;
}

//It parses the HTTP request method, URI, HTTP protocol version, and headers
int processRequest(Request *req) {
    int parse_result = 0;

    // Parse HTTP request method
    if (parseRequestMethod(req) < 0 && req != 0) {
        fprintf(stderr, "Failed to parse HTTP request method\n");
        parse_result = -1;
        return parse_result; // Add explicit return after error handling
    }

    // Parse URI from the request
    if (parseRequestURI(req) < 0 && req != 0) {
        fprintf(stderr, "Failed to parse URI\n");
        parse_result = -1;
        return parse_result;
    }

    // Parse HTTP protocol version
    if (parse_http_protocol_version(req) < 0 && req != 0) {
        fprintf(stderr, "Failed to parse HTTP protocol version\n");
        parse_result = -1;
        return parse_result;
    }

    // Receive data into the input buffer
    const ssize_t rb = recv(req->sockfd, req->input_buffer.buf + req->input_buffer.write_position,
        MAX_REQUEST_SIZE - req->input_buffer.write_position, 0);
    if (rb > 0) {
        req->input_buffer.write_position += rb;
    } else if (rb == -1) {
        fprintf(stderr, "Error receiving data\n");
        parse_result = -1;
        return parse_result;
    }

    // Validate and parse headers
    if (parse_and_validate_headers(req) < 0 && req != 0) {
        fprintf(stderr, "Failed to parse and validate headers\n");
        parse_result = -1;
        return parse_result;
    }

    // Process request body
    if (process_request_body(req) < 0 && req != 0) {
        fprintf(stderr, "Failed to process request body\n");
        parse_result = -1;
        return parse_result;
    }

    return parse_result;
}

//retrieve the socket file descriptor from an HTTP request object
int get_socket_fd(const Request *req) {
    if (req == NULL) {
        fprintf(stderr, "Error: NULL request object passed to http_request_get_sockfd\n");
        return -1; // Return an error code to indicate the failure
    }

    return req->sockfd; // Return the socket file descriptor stored in the request object
}

RequestType retrieve_http_request_method(const Request *req) {
    if (req != NULL) {
        return req->request_method;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_method\n");
        return HTTP_UNSUPPORTED; // Return a default value or error code
    }
}

char *get_request_uri(const Request *req) {
    if (req != NULL) {
        return req->request_uri;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_uri\n");
        return NULL;
    }
}

char get_http_version_major(const Request *req) {
    if (req != NULL) {
        return req->http_version_major;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_http_ver_major\n");
        return '0'; // Return a default value or error code
    }
}

char get_http_version_minor(const Request *req) {
    if (req != NULL) {
        return req->http_version_minor;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_http_ver_minor\n");
        return '0'; // Return a default value or error code
    }
}

int get_number_of_headers(const Request *req) {
    if (req != NULL) {
        return req->number_of_headers;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_num_headers\n");
        return 0; // Return a default value or error code
    }
}

KeyValue *get_request_headers(const Request *req) {
    if (req != NULL) {
        return req->request_headers;
    } else {
        // Handle NULL request object error
        fprintf(stderr, "Error: NULL request object in http_request_get_headers\n");
        return NULL; // Return NULL to indicate an error
    }
}

// Function to retrieve the header value based on the header key from the request headers array
char *get_dah_header_value(const Request *req, const char *HeaderKey) {
    int i = 0;
    // Iterate through the request headers array
    do {
        // Check if the current header key matches the provided HeaderKey
        if (strcasecmp(req->request_headers[i].HeaderKey, HeaderKey) == 0) {
            return req->request_headers[i].HeaderValue; // Return the header value if key matches
        }
        i++;
    } while (i < req->number_of_headers); // Continue until all headers are checked

    return NULL; // Return NULL if the header key is not found in the request headers
}

// converts a string to a long integer
ssize_t convert_string_to_long(const char *str) {
    ssize_t res = 0; // Initialize the result to 0
    for (unsigned int i = 0; str[i] != '\0'; i++) { // Iterate through each character in the string
        res *= 10; // Multiply the result by 10 for each digit
        // Check each character and add the corresponding value to the result
        if (str[i] == '1') {
            res += 1;
        } else if (str[i] == '2') {
            res += 2;
        } else if (str[i] == '3') {
            res += 3;
        } else if (str[i] == '4') {
            res += 4;
        } else if (str[i] == '5') {
            res += 5;
        } else if (str[i] == '6') {
            res += 6;
        } else if (str[i] == '7') {
            res += 7;
        } else if (str[i] == '8') {
            res += 8;
        } else if (str[i] == '9') {
            res += 9;
        } else if (str[i] == '0') {
            continue; // Skip if the character is '0'
        } else {
            return -1; // Return -1 if a non-digit character is encountered
        }
    }
    return res; // Return the converted long integer value
}

//retrieves and processes the Content-Length header value from an HTTP request, converts it to a long integer, and performs validation checks on the content length
ssize_t new_handle_connection(const Request *req) {
    const char *content_length_str = get_dah_header_value(req, "Content-Length");
    if (content_length_str == NULL) {
        return -1; // Return -1 if Content-Length header is missing
    }

    // Convert the header value to a long integer
    ssize_t content_length = convert_string_to_long(content_length_str);
    if (content_length < 0) {
        return -2; // Return -2 if conversion to long integer fails
    }

    // Additional check for negative content length
    if (content_length < 0) {
        return -3; // Return -3 for negative content length
    }

    return content_length;
}

// fetch the size of the request body from an HTTP request object
Buffer_Size fetchRequestBodySize(const Request *req) {
    // Check if the request object is NULL
    if (req == NULL) {
        fprintf(stderr, "Error: NULL request object in fetchRequestBodySize\n");
        return -1; // Return an error code to indicate the failure
    }
    return req->request_body_size;
}

// retrieve the body content from an HTTP request object
char *fetchRequestBody(const Request *req) {
    // Check if the request object is NULL
    if (req == NULL) {
        fprintf(stderr, "Error: NULL request object in fetchRequestBody\n");
        return NULL; // Return NULL to indicate an error
    }
    return req->request_body; // Return the pointer to the body content of the request
}

// clean up the initialized regex patterns
void clear_http_regex() {
    int i = 0; // Initialize the counter variable
    do {
        if (i < 5) { // Check if the index is within the bounds of the _regs array
            regfree(&_regs[i]); // Free the memory allocated for the regex pattern at index i
        } else {
            fprintf(stderr,
                "Error: Index out of bounds in clear_http_regex\n"); // Print an error message for out of bounds index
        }
        i++; // Increment the counter variable
    } while (i < regexInitialized); // Continue until all initialized regex patterns are cleaned up
}

int http_regex_init() {
    INIT_REGEX(METHOD);
    INIT_REGEX(URI);
    INIT_REGEX(HTTP_VERSION);
    INIT_REGEX(HEADERS);
    INIT_REGEX(HEADER);
    return 0;
}

// find or create a file lock based on the URI
static struct file_lock *find_file_lock(const char *URI) {
    // Check if the URI is NULL
    if (URI == NULL) {
        fprintf(stderr, "Error: NULL URI passed to find_file_lock\n");
        return NULL; // Return NULL if URI is NULL
    }

    pthread_mutex_lock(&file_locks_mutex); // Lock the mutex for thread safety
    int i = 0;
    struct file_lock *found_lock = NULL;

    // Iterate through the file locks array
    do {
        // Check if the index exceeds the maximum thread count
        if (i >= thread_count) {
            fprintf(stderr, "Error: Exceeded maximum thread count in find_file_lock\n");
            break; // Break the loop if maximum count exceeded
        }

        // Check if the file lock at index i is available
        if (file_locks[i].filename == NULL) {
            file_locks[i].filename = strdup(URI); // Set the filename to the URI
            file_locks[i].users = 1; // Set the number of users to 1
            found_lock = &file_locks[i]; // Assign the found lock
            break; // Break the loop after finding a lock
        } else if (strcmp(file_locks[i].filename, URI) == 0) {
            file_locks[i].users++; // Increment the number of users for an existing lock
            found_lock = &file_locks[i]; // Assign the found lock
            break; // Break the loop after finding a lock
        }

        i++; // Move to the next index
    } while (1); // Infinite loop for searching or creating a lock

    pthread_mutex_unlock(&file_locks_mutex); // Unlock the mutex after processing

    // Check if a lock was not found for the URI
    if (found_lock == NULL) {
        fprintf(stderr, "Error: File lock not found for URI: %s\n", URI);
    }

    return found_lock; // Return the found or created file lock
}

//release a file lock by decrementing the users count and freeing resources if necessary
static void release_file_lock(struct file_lock *lock) {
    // Check if the file lock is NULL
    if (lock == NULL) {
        fprintf(stderr, "Error: NULL file lock passed to release_file_lock\n");
        return; // Return if the file lock is NULL
    }

    // Lock the mutex for thread safety
    if (pthread_mutex_lock(&file_locks_mutex) != 0) {
        fprintf(stderr, "Error: Failed to lock the mutex in release_file_lock\n");
        return; // Return if mutex locking fails
    }

    // Decrement the users count
    lock->users--;

    // Check if users count is negative and reset to 0 if negative
    if (lock->users < 0) {
        fprintf(stderr, "Error: Negative users count in release_file_lock\n");
        lock->users = 0; // Reset users count to 0 to avoid negative values
    }

    // Check if users count is zero before freeing resources
    if (lock->users == 0) {
        if (lock->filename != NULL) {
            free(lock->filename); // Free the filename memory if not NULL
            lock->filename = NULL; // Set filename pointer to NULL after freeing
        } else {
            fprintf(stderr, "Error: Attempting to free NULL filename in release_file_lock\n");
        }
    }

    // Unlock the mutex after processing
    if (pthread_mutex_unlock(&file_locks_mutex) != 0) {
        fprintf(stderr, "Error: Failed to unlock the mutex in release_file_lock\n");
    }
}

// write audit log information based on operation, URI, response status, and request ID
static void write_audit_log(
    const char *op, const char *URI, const int response_status, const char *req_id) {
    // Check if any of the input parameters are NULL
    if (op == NULL || URI == NULL || req_id == NULL) {
        fprintf(stderr, "Error: Null parameter passed to write_audit_log\n");
        return; // Return if any of the input parameters are NULL
    }

    // Check if the response status is within the valid range of HTTP status codes (100-599)
    if (response_status < 100 || response_status > 599) {
        fprintf(stderr, "Error: Invalid response status code\n");
        return; // Return if the response status is outside the valid range
    }

    // Additional check for response status outside the valid HTTP status code range (100-599)
    if (response_status < 100 || response_status > 999) {
        fprintf(stderr, "Error: Invalid HTTP response status code: %d\n", response_status);
        return; // Return if the response status is not a valid HTTP status code
    }

    // Print the audit log information with operation, URI, response status, and request ID
    fprintf(stderr, "%s,/%s,%d,%s\n", op, URI, response_status, req_id);
}

ServerResponse handle_get(const Request *req) {
    const char *URI = get_request_uri(req);
    if (URI == NULL) {
        return CREATE_UNSENT_RESPONSE(400); // Bad Request if URI is NULL
    }

    // Attempt to open the file specified by the URI in read-only mode
    const int fd = open(URI, O_RDONLY);
    // Check if the file descriptor is invalid (open failed)
    if (fd == -1) {
        // Check different error conditions based on errno values
        if (errno == EACCES || errno == ENAMETOOLONG || errno == EPERM || errno == EROFS) {
            return CREATE_UNSENT_RESPONSE(
                403); // Return 403 Forbidden response for access or permission errors
        } else if (errno == ENOENT) {
            return CREATE_UNSENT_RESPONSE(
                404); // Return 404 Not Found response if the file does not exist
        } else {
            return CREATE_UNSENT_RESPONSE(
                500); // Return 500 Internal Server Error for other open errors
        }
    }

    // Retrieve file status information using fstat
    struct stat st;
    const int st_res = fstat(fd, &st);

    // Check if fstat operation failed
    if (st_res == -1) {
        // Handle different error cases based on errno values
        if (errno == EACCES || errno == EBADF || errno == EFAULT) {
            close(fd);
            return CREATE_UNSENT_RESPONSE(403); // Return 403 Forbidden response
        } else if (errno == ENOENT) {
            close(fd);
            return CREATE_UNSENT_RESPONSE(404); // Return 404 Not Found response
        } else {
            close(fd);
            return CREATE_UNSENT_RESPONSE(
                500); // Return 500 Internal Server Error response for other errors
        }
    }

    if (!S_ISREG(st.st_mode)) {
        close(fd);
        return CREATE_UNSENT_RESPONSE(403); // Forbidden if not a regular file
    }

    const off_t file_size = st.st_size;
    if (file_size <= 0) {
        close(fd);
        return CREATE_UNSENT_RESPONSE(500); // Internal Server Error for empty file
    }

    // Retrieve the socket file descriptor from the HTTP request object
    const int sock = get_socket_fd(req);
    // Write the HTTP response status line to the socket
    write_n_bytes(sock, "HTTP/1.1 200 OK\r\n", 17);
    // Prepare the Content-Length header value string
    char file_size_str[64];
    sprintf(file_size_str, "Content-Length: %lu\r\n", file_size);
    // Write the Content-Length header to the socket
    write_n_bytes(sock, file_size_str, strlen(file_size_str));
    // Write the empty line to indicate the end of headers
    write_n_bytes(sock, "\r\n", 2);

    // Check for errors during data transmission
    if (pass_n_bytes(fd, sock, file_size) == -1) {
        close(fd);
        return CREATE_UNSENT_RESPONSE(
            500); // Return 500 Internal Server Error on transmission error
    }

    close(fd);
    return CREATE_SENT_RESPONSE(200);
}

// Handle the PUT request by processing the request object
ServerResponse handle_put(Request *req) {
    // Get the content length of the request
    const ssize_t content_length = new_handle_connection(req);
    // Check if content length is invalid and return appropriate response
    if (content_length < 0) {
        return CREATE_UNSENT_RESPONSE(400);
    }

    // Get the URI from the request object
    const char *URI = get_request_uri(req);
    // Get the size of the request body
    const Buffer_Size request_body_size = fetchRequestBodySize(req);

    // Open or create a file based on the URI
    int fd = open(URI, O_WRONLY | O_TRUNC, 0);
    int res;
    if (fd == -1) {
        // Handle different file open errors and set response status
        if (errno == EISDIR || errno == EACCES || errno == ENAMETOOLONG || errno == EPERM
            || errno == EROFS) {
            return CREATE_UNSENT_RESPONSE(403);
        } else if (errno == ENOENT) {
            fd = creat(URI, 0666);
            res = 201;
        } else {
            return CREATE_UNSENT_RESPONSE(500);
        }
    } else {
        res = 200;
    }

    // Check if content length is 0 and close the file descriptor
    if (content_length == 0) {
        close(fd);
        return CREATE_UNSENT_RESPONSE(res);
    }

    ssize_t total_wb = 0;
    ssize_t wb;
    // Write the request body to the file if body size is greater than 0
    if (request_body_size > 0) {
        char *request_body = fetchRequestBody(req);
        wb = write_n_bytes(fd, request_body, request_body_size);
        total_wb += wb;
    }

    // Check if total bytes written match content length and close the file
    if (total_wb == content_length) {
        close(fd);
        return CREATE_UNSENT_RESPONSE(res);
    }

    // Get the socket file descriptor and pass remaining bytes to the socket
    const int sock = get_socket_fd(req);
    pass_n_bytes(sock, fd, content_length - total_wb);
    close(fd);
    return CREATE_UNSENT_RESPONSE(res);
}

// Handle the incoming HTTP request by processing the request object and generating a server response
ServerResponse handle_connection(Request *req) {
    // Process the request and check for any errors
    if (processRequest(req) != 0) {
        return CREATE_UNSENT_RESPONSE(400); // Return Bad Request response
    }

    // Retrieve the Request ID from the headers
    const char *request_id = get_dah_header_value(req, "Request-Id");
    if (request_id == NULL) {
        return CREATE_UNSENT_RESPONSE(400); // Return Bad Request response if Request ID is missing
    }

    // Retrieve the URI and request method
    const char *URI = get_request_uri(req);
    RequestType req_method = retrieve_http_request_method(req);

    // Handle different HTTP request methods
    ServerResponse server_response;
    struct file_lock *lock;

    if (req_method == HTTP_GET) {
        lock = find_file_lock(URI); // Find or create a file lock based on the URI
        reader_lock(lock->lock); // Acquire a reader lock
        server_response = handle_get(req); // Handle the GET request
        write_audit_log("GET", URI, server_response.response_status,
            request_id); // Write audit log for GET request
        reader_unlock(lock->lock); // Release the reader lock
        release_file_lock(lock); // Release the file lock
    } else if (req_method == HTTP_PUT) {
        lock = find_file_lock(URI); // Find or create a file lock based on the URI
        writer_lock(lock->lock); // Acquire a writer lock
        server_response = handle_put(req); // Handle the PUT request
        write_audit_log("PUT", URI, server_response.response_status,
            request_id); // Write audit log for PUT request
        writer_unlock(lock->lock); // Release the writer lock
        release_file_lock(lock); // Release the file lock
    } else {
        return CREATE_UNSENT_RESPONSE(
            501); // Return Not Implemented response for unsupported method
    }

    return server_response; // Return the server response
}

// generate an HTTP response based on the provided response status code
void respond(const int conn, const int response_status) {
    char *response_status_line, *request_body;

    // Assign response status line and body based on the response status code
    if (response_status == 200) {
        response_status_line = "200 OK";
        request_body = "OK\n";
    } else if (response_status == 201) {
        response_status_line = "201 Created";
        request_body = "Created\n";
    } else if (response_status == 400) {
        response_status_line = "400 Bad Request";
        request_body = "Bad Request\n";
    } else if (response_status == 403) {
        response_status_line = "403 Forbidden";
        request_body = "Forbidden\n";
    } else if (response_status == 404) {
        response_status_line = "404 Not Found";
        request_body = "Not Found\n";
    } else if (response_status == 501) {
        response_status_line = "501 Not Implemented";
        request_body = "Not Implemented\n";
    } else if (response_status == 505) {
        response_status_line = "505 Version Not Supported";
        request_body = "Version Not Supported\n";
    } else {
        response_status_line = "500 Internal Server Error";
        request_body = "Internal Server Error\n";
    }

    // Additional error handling for unexpected response_status values
    if (response_status < 200 || response_status > 505) {
        response_status_line = "500 Internal Server Error";
        request_body = "Unexpected Error\n";
    }

    // Additional error handling based on specific conditions
    if (response_status == 503) {
        response_status_line = "503 Service Unavailable";
        request_body = "Service Unavailable\n";
    } else if (response_status == 504) {
        response_status_line = "504 Gateway Timeout";
        request_body = "Gateway Timeout\n";
    }

    // Write the HTTP response to the connection socket
    write_n_bytes(conn, "HTTP/1.1 ", 9);
    // Write the response status line to the connection socket
    write_n_bytes(conn, response_status_line, strlen(response_status_line));
    // Write the line break to the connection socket
    write_n_bytes(conn, "\r\n", 2);
    // Write the Content-Length header label to the connection socket
    write_n_bytes(conn, "Content-Length: ", 16);
    // Prepare the Content-Length value string
    char content_len[130];
    sprintf(content_len, "%lu\r\n", strlen(request_body));
    // Write the Content-Length value to the connection socket
    write_n_bytes(conn, content_len, strlen(content_len));
    // Write another line break to the connection socket
    write_n_bytes(conn, "\r\n", 2);
    // Write the request body content to the connection socket
    write_n_bytes(conn, request_body, strlen(request_body));
}

// Signal handler function to handle specific signals
static void signal_handler(const int n) {
    // Check if the signal is SIGINT or SIGTERM
    if (n == SIGINT || n == SIGTERM) {
        // Close the socket file descriptor if the signal is SIGINT or SIGTERM
        if (close(sock.fd) == -1) {
            fprintf(stderr, "Error: Failed to close socket file descriptor\n");
        }
    } else {
        int i = 0;
        // Loop through the threads array to cancel each thread
        do {
            // Attempt to cancel the thread at index i
            if (pthread_cancel(threads_arr[i]) != 0) {
                fprintf(stderr, "Error: Failed to cancel thread %d\n", i);
            }
            i++;
        } while (i < thread_count); // Continue until all threads are canceled
        running = false; // Set the running flag to false to control the main loop execution
    }
}

// parse command line arguments for port and thread count
static void parse_command(const int argc, char *const *argv, int *port, int *threads) {
    // Get the option character from the command line arguments
    int opt = getopt(argc, argv, "t:");
    // Check if the option is 't' for thread count
    if (opt == 't') {
        // Read and store the thread count value
        if (sscanf(optarg, "%d", threads) != 1) {
            fprintf(stderr, "Invalid thread count: %s\n", optarg);
            exit(1);
        }
    } else if (opt == '?') {
        // Print usage message for incorrect option
        fprintf(stderr, "Usage: %s [-t threads] <port>\n", optarg);
        exit(1);
    } else {
        *threads = 4; // Default thread count if option not provided
    }

    // Check if there are enough arguments for port
    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
        exit(1);
    }

    // Read and store the port value
    if (sscanf(argv[optind], "%d", port) != 1) {
        fprintf(stderr, "Invalid port: %s\n", argv[optind]);
        exit(1);
    }

    // Check for valid thread count and port range
    if (*threads <= 0) {
        fprintf(stderr, "Invalid thread count: %d\n", *threads);
        exit(1);
    }

    // Check if the port value is within the valid range of 1 to 65535
    if (*port < 1 || *port > 65535) {
        fprintf(stderr, "Invalid port: %d\n", *port);
        exit(1);
    }
}

// Define a worker thread function that processes requests from a queue
void *worker_thread(void *arg) {
    // Cast the argument as a queue pointer
    queue_t *queue = arg;
    Request *req;

    // Start an infinite loop to continuously process requests
    do {
        // Pop a request from the queue and assign it to req
        queue_pop(queue, (void **) &req);

        // Handle the connection request and get the server response
        ServerResponse server_response = handle_connection(req);

        // Check if the server response has been sent
        if (server_response.has_responded) {
            //pog
        } else {
            // Perform an action when the server response has not been sent
            // Send a response to the client with the socket file descriptor and response status
            respond(get_socket_fd(req), server_response.response_status);
        }

        // Close the request socket file descriptor
        http_request_close(req);

        // Free the memory allocated for the request object
        http_request_free(req);
    } while (true); // Continue processing requests in an infinite loop

    return NULL; // Return NULL after processing
}

// main
int main(const int argc, char *const argv[]) {
    int port, threads;
    parse_command(argc, argv, &port, &threads);

    // Switch statement to handle different port validation cases
    switch (port) {
    case 0: fprintf(stderr, "Port cannot be 0\n"); return 1;
    case 65535: fprintf(stderr, "Port 65535 is reserved\n"); return 1;
    default:
        // Handle other port validation cases
        sock.fd = 0;
        if (listener_init(&sock, port) == -1) {
            fprintf(stderr, "Invalid port: %d\n", port);
            return 1;
        }
    }

    // Set up signal handlers for SIGINT and SIGTERM
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Switch statement to handle the initialization of regex patterns
    switch (http_regex_init()) {
    case 0:
        // Regex initialization successful
        break;
    case -1: fprintf(stderr, "Failed to initialize regex: Error code -1\n"); return 1;
    case -2: fprintf(stderr, "Failed to initialize regex: Error code -2\n"); return 1;
    default: fprintf(stderr, "Unknown error during regex initialization\n"); return 1;
    }

    // Create a new queue with the specified number of threads
    queue_t *queue = queue_new(threads);
    pthread_t _real_threads_array_but_its_on_the_stack[threads];
    threads_arr = _real_threads_array_but_its_on_the_stack;
    thread_count = threads;
    struct file_lock _real_file_locks_array_but_its_on_the_stack[threads];
    file_locks = _real_file_locks_array_but_its_on_the_stack;

    // Create worker threads and initialize file locks
    int i = 0;
    do {
        pthread_create(&threads_arr[i], NULL, worker_thread, queue);
        file_locks[i].lock = rwlock_new(N_WAY, 1);
        file_locks[i].filename = NULL;
        file_locks[i].users = 0;
        i++;
    } while (i < threads);

    // Accept incoming connections and push requests to the queue
    int conn;
    do {
        if ((conn = listener_accept(&sock)) != -1) {
            Request *req = http_request_create(conn);
            queue_push(queue, req);
        }
    } while (running);

    // Join threads and clean up resources
    int j = 0;
    do {
        pthread_join(threads_arr[j], NULL);
        rwlock_delete(&file_locks[j].lock);
        j++;
    } while (j < threads);

    // Delete the queue and clear regex patterns
    queue_delete(&queue);
    clear_http_regex();

    return 0;
}
