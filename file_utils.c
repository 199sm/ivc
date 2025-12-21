#include "file_utils.h"

#define COPY_BUFFER_SIZE 8192

int copy_file(const char *source_path, const char *dest_path) {
    FILE *source = fopen(source_path, "rb");
    if (source == NULL) {
        fprintf(stderr, "Failed to open source file: %s\n", source_path);
        return 1;
    }

    FILE *dest = fopen(dest_path, "wb");
    if (dest == NULL) {
        fprintf(stderr, "Failed to open destination file: %s\n", dest_path);
        fclose(source);
        return 1;
    }

    char buffer[COPY_BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, COPY_BUFFER_SIZE, source)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest) != bytes_read) {
            fprintf(stderr, "Error writing to destination file\n");
            fclose(source);
            fclose(dest);
            return 1;
        }
    }

    if (ferror(source)) {
        fprintf(stderr, "Error reading from source file\n");
        fclose(source);
        fclose(dest);
        return 1;
    }

    fclose(source);
    fclose(dest);

    return 0;
}