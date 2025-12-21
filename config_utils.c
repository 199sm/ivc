#include "config_utils.h"

inline int read_config(const char *config_path, yaml_document_t *doc) {
    FILE *config_file = fopen(config_path, "r");
    if (!config_file) {
        fprintf(stderr, "Error: Configuration file not found at %s\n", config_path);
        return 0;
    }

    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, config_file);

    if (!yaml_parser_load(&parser, doc)) {
        fprintf(stderr, "Error parsing YAML.\n");
        yaml_parser_delete(&parser);
        fclose(config_file);
        return 0;
    }

    yaml_parser_delete(&parser);
    fclose(config_file);
    return 1;
}