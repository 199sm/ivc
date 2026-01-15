#ifndef OPTIONS_H
#define OPTIONS_H

#include "stdio.h"
#include "stdlib.h"
#include "getopt.h"
#include "string.h"

struct options {
    char *template_path;
    char *config_path;
    char *output_path;
    int exit_code;
};

struct options *get_options(int argc, char *argv[]);

#endif //OPTIONS_H
