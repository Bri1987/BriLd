#include "util.h"

void fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("fatal: ");
    vprintf(format, args);
    printf("\n");

    va_end(args);

    exit(1);
}

// Function to read the entire content of a file into a dynamically allocated buffer
char* ReadFile(const char* filename) {
    FILE* file;
    char* buffer;
    size_t file_size;

    // Open the file
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory for the entire content
    buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    if (fread(buffer, 1, file_size, file) != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate the buffer
    buffer[file_size] = '\0';

    // Close the file
    fclose(file);
    return buffer;
}

void Read(void* out, const void* data, size_t size) {
    // Assuming little-endian format
    memcpy(out, data, size);
}