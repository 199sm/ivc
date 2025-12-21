#include "unzip_utils.h"

// Function to map libzip error codes to human-readable error messages
inline const char *zip_error_to_string(const int err_code) {
    switch (err_code) {
        case ZIP_ER_EXISTS:
            return "The file specified by path exists and ZIP_EXCL is set.";
        case ZIP_ER_INCONS:
            return
                    "Inconsistencies were found in the file specified by path. This error is often caused by specifying ZIP_CHECKCONS but can also happen without it.";
        case ZIP_ER_INVAL:
            return "The path argument is NULL.";
        case ZIP_ER_MEMORY:
            return "Required memory could not be allocated.";
        case ZIP_ER_NOENT:
            return "The file specified by path does not exist and ZIP_CREATE is not set.";
        case ZIP_ER_NOZIP:
            return "The file specified by path is not a zip archive.";
        case ZIP_ER_OPEN:
            return "The file specified by path could not be opened.";
        case ZIP_ER_READ:
            return "A read error occurred; see errno for details.";
        case ZIP_ER_SEEK:
            return "The file specified by path does not allow seeks.";
        default:
            return "Unknown error code.";
    }
}

// Function to unzip a specific file from a zip archive to memory
inline char *unzip_file_to_memory(const char *zip_filepath, const char *target_filename) {
    int err = 0;
    struct zip *archive = zip_open(zip_filepath, 0, &err);
    if (archive == NULL) {
        fprintf(stderr, "Failed to open %s\nError: %d - %s\n", zip_filepath, err, zip_error_to_string(err));
        return NULL;
    }

    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(archive, target_filename, 0, &st) != 0) {
        fprintf(stderr, "Error finding file %s in archive %s\n", target_filename, zip_filepath);
        zip_close(archive);
        return NULL;
    }

    const zip_uint64_t size = st.size;
    char *buffer = malloc(size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        zip_close(archive);
        return NULL;
    }

    struct zip_file *zf = zip_fopen(archive, target_filename, 0);
    if (zf == NULL) {
        fprintf(stderr, "Failed to open file %s in archive: %s\n", target_filename, zip_strerror(archive));
        free(buffer);
        zip_close(archive);
        return NULL;
    }

    const zip_int64_t bytes_read = zip_fread(zf, buffer, size);
    if (bytes_read < 0 || (zip_uint64_t) bytes_read != size) {
        fprintf(stderr, "Error reading file %s\n", target_filename);
        free(buffer);
        zip_fclose(zf);
        zip_close(archive);
        return NULL;
    }

    zip_fclose(zf);
    zip_close(archive);

    buffer[size] = '\0';
    return buffer;
}

// Function to write content to zip archive
inline int write_file_to_zip(const char *zip_filepath, const char *target_filename, const char *new_content) {
    int err = 0;

    // Open ZIP for modification
    struct zip *archive = zip_open(zip_filepath, 0, &err);
    if (archive == NULL) {
        fprintf(stderr, "Failed to open %s\nError: %d - %s\n",
                zip_filepath, err, zip_error_to_string(err));
        return 1;
    }

    // Find and remove the old file
    const zip_int64_t file_index = zip_name_locate(archive, target_filename, 0);
    if (file_index < 0) {
        fprintf(stderr, "File %s not found in archive\n", target_filename);
        zip_close(archive);
        return 1;
    }

    if (zip_delete(archive, file_index) != 0) {
        fprintf(stderr, "Failed to delete %s from archive: %s\n",
                target_filename, zip_strerror(archive));
        zip_close(archive);
        return 1;
    }

    // Create new source from modified content
    const zip_uint64_t content_size = strlen(new_content);
    struct zip_source *source = zip_source_buffer(archive, new_content, content_size, 0);

    if (source == NULL) {
        fprintf(stderr, "Failed to create source buffer: %s\n", zip_strerror(archive));
        zip_close(archive);
        return 1;
    }

    // Add the new file
    if (zip_file_add(archive, target_filename, source, 0) < 0) {
        fprintf(stderr, "Failed to add %s to archive: %s\n",
                target_filename, zip_strerror(archive));
        zip_source_free(source);
        zip_close(archive);
        return 1;
    }

    // Write changes to disk
    if (zip_close(archive) != 0) {
        fprintf(stderr, "Failed to close/save archive\n");
        return 1;
    }

    return 0;
}
