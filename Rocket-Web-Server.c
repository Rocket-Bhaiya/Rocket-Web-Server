#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8888

// Function to print the help message
void print_help() {
    printf("Usage: Rocket-Web-Server [options]\n");
    printf("Options:\n");
    printf("  --help           Show this help message\n");
    printf("  --port <port>    Set the server port (default: 8888)\n");
    printf("  --file <file>    Set the file to serve at the root (default: index.php)\n");
}

// Function to run PHP file and capture the output
char *execute_php(const char *filename) {
    char command[256];
    snprintf(command, sizeof(command), "php %s", filename);

    // Execute the PHP script and capture the output
    FILE *fp = popen(command, "r");
    if (!fp) {
        return NULL;
    }

    // Read the output of PHP
    char *output = malloc(1024);
    size_t len = fread(output, 1, 1024, fp);
    output[len] = '\0';
    
    fclose(fp);
    return output;
}

// Function to handle HTTP requests
static enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                            const char *url, const char *method,
                                            const char *version, const char *upload_data,
                                            size_t *upload_data_size, void **con_cls) {
    char *page; // Changed to char * to allow modification
    const char *file_to_serve = (const char *)cls;

    // Check for the requested URL
    if (strcmp(url, "/") == 0) {
        // Serve the specified file
        if (strstr(file_to_serve, ".php")) {
            // Serve the PHP file
            page = execute_php(file_to_serve);
            if (!page) {
                page = "<html><body><h1>500 Internal Server Error</h1></body></html>";
                return MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT));
            }
        } else if (strstr(file_to_serve, ".html")) {
            // Serve the HTML file (static)
            FILE *file = fopen(file_to_serve, "r");
            if (!file) {
                page = "<html><body><h1>404 Not Found</h1></body></html>";
            } else {
                fseek(file, 0, SEEK_END);
                long size = ftell(file);
                fseek(file, 0, SEEK_SET);
                page = malloc(size + 1); // Ensure enough space for content + null terminator
                fread(page, 1, size, file);
                page[size] = '\0'; // Null-terminate the string
                fclose(file);
            }
        } else {
            // Serve 404 if the file type is not recognized
            page = "<html><body><h1>404 Not Found</h1></body></html>";
        }
    } else {
        // Handle other URLs as 404
        page = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    struct MHD_Response *response;
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Content-Type", "text/html");
    
    // Send the response
    MHD_queue_response(connection, MHD_HTTP_OK, response);
    
    // Cleanup response after sending
    MHD_destroy_response(response);
    
    return MHD_YES;
}

int main(int argc, char *argv[]) {
    int port = PORT;
    char *file_to_serve = "index.php"; // Default file to serve
    struct MHD_Daemon *daemon = NULL;

    // Check for command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0; // Exit after printing help
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = atoi(argv[i + 1]);
            i++; // Skip the next argument since it's the port number
        } else if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            file_to_serve = argv[i + 1];
            i++; // Skip the next argument since it's the file to serve
        }
    }

    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL,
                              &answer_to_connection, file_to_serve, MHD_OPTION_END);
    if (NULL == daemon) {
        return 1;
    }

    printf("Server started on port %d, serving file: %s\n", port, file_to_serve);
    getchar(); // Wait until user presses Enter

    MHD_stop_daemon(daemon);
    return 0;
}
