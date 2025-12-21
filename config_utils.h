#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include <yaml.h>

int read_config(const char *config_path, yaml_document_t *doc);

#endif // CONFIG_UTILS_H
