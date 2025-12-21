#ifndef TEMPLATE_UTILS_H
#define TEMPLATE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *replace_template(char *content, const char *pattern, const char *value);

char *get_last_w_tr_block(const char *content);

char *copy_string(const char *source);

char *concat(const char *str1, const char *str2);

#endif // TEMPLATE_UTILS_H
