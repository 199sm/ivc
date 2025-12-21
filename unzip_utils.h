#ifndef UNZIP_UTILS_H
#define UNZIP_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

// Function to map libzip error codes to human-readable error messages
const char *zip_error_to_string(int err_code);

// Function to unzip a specific file from a .docx (zip) archive to memory
char *unzip_file_to_memory(const char *zip_filepath, const char *target_filename);

// Function to write content to zip archive
int write_file_to_zip(const char *zip_filepath, const char *target_filename, const char *new_content);

#endif // UNZIP_UTILS_H
