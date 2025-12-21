#include "template_utils.h"

inline char *replace_template(char *content, const char *pattern, const char *value) {
    if (content == NULL || pattern == NULL || value == NULL)
        return content;

    const long pattern_len = strlen(pattern);
    const long value_len = strlen(value);
    const long content_len = strlen(content);

    int count = 0;
    for (long i = 0; i + pattern_len <= content_len; i++) {
        if (strncmp(&content[i], pattern, pattern_len) == 0) {
            count++;
            i += pattern_len - 1;
        }
    }

    if (count == 0)
        return content;

    const long new_size = content_len + count * (value_len - pattern_len) + 1;
    char *result = malloc(new_size);

    if (result == NULL) {
        perror("malloc failed");
        return content;
    }

    long result_pos = 0;
    long i = 0;

    while (i < content_len) {
        if (i + pattern_len <= content_len &&
            strncmp(&content[i], pattern, pattern_len) == 0) {
            strcpy(&result[result_pos], value);
            result_pos += value_len;
            i += pattern_len;
            } else {
                result[result_pos++] = content[i++];
            }
    }

    result[result_pos] = '\0';
    free(content);
    return result;
}

inline char* get_last_w_tr_block(const char *content) {
    if (content == NULL)
        return NULL;

    const char *last_opening = NULL;
    const char *search_pos = content;

    // Find the last occurrence of <w:tr>
    while ((search_pos = strstr(search_pos, "<w:tr")) != NULL) {
        // Check if it's an opening tag, not closing
        if (search_pos[5] == '>' || search_pos[5] == ' ') {
            last_opening = search_pos;
        }
        search_pos++;
    }

    if (last_opening == NULL)
        return NULL;

    // Find closing tag </w:tr> after the last opening
    const char *closing = strstr(last_opening, "</w:tr>");
    if (closing == NULL)
        return NULL;

    // Calculate block length
    size_t block_length = (closing + 7) - last_opening; // 7 = strlen("</w:tr>")

    // Allocate and copy block
    char *block = malloc(block_length + 1);
    if (block == NULL) {
        perror("malloc failed");
        return NULL;
    }

    strncpy(block, last_opening, block_length);
    block[block_length] = '\0';

    return block;
}

inline char* copy_string(const char *source) {
    if (source == NULL)
        return NULL;

    size_t len = strlen(source);
    char *dest = malloc(len + 1);

    if (dest == NULL) {
        perror("malloc failed");
        return NULL;
    }

    strcpy(dest, source);
    return dest;
}

inline char* concat(const char *str1, const char *str2) {
    if (str1 == NULL || str2 == NULL)
        return NULL;

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total = len1 + len2 + 1; // +1 for null terminator

    char *result = malloc(total);

    if (result == NULL) {
        perror("malloc failed");
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}