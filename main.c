#include "config_utils.h"
#include "file_utils.h"
#include "template_utils.h"
#include "unzip_utils.h"
#include "options.h"

int main(int argc, char *argv[]) {
    const struct options* opts = get_options(argc, argv);

    if (opts == NULL) {
        return EXIT_FAILURE;
    }

    if (opts->exit_code >= 0) {
        return opts->exit_code;
    }

    const char *target_file = "word/document.xml";

    const int copy_result = copy_file(opts->template_path, opts->output_path);
    if (copy_result != 0) {
        fprintf(stderr, "Error copying file: %s\n", opts->output_path);
        return EXIT_FAILURE;
    }

    char *content = unzip_file_to_memory(opts->output_path, target_file);

    if (content == NULL)
        return EXIT_FAILURE;

    yaml_document_t doc;

    if (read_config(opts->config_path, &doc)) {
        const yaml_node_t *root = yaml_document_get_root_node(&doc);
        if (root && root->type == YAML_MAPPING_NODE) {
            for (const yaml_node_pair_t *pair = root->data.mapping.pairs.start;
                 pair < root->data.mapping.pairs.top;
                 pair++) {
                const yaml_node_t *key_node = yaml_document_get_node(&doc, pair->key);
                const yaml_node_t *value_node = yaml_document_get_node(&doc, pair->value);

                if (key_node->type == YAML_SCALAR_NODE &&
                    value_node->type == YAML_SCALAR_NODE) {
                    const char *key = (const char *) key_node->data.scalar.value;
                    const char *value = (const char *) value_node->data.scalar.value;
                    char pattern[512];
                    snprintf(pattern, sizeof(pattern), "{{ %s }}", key);
                    content = replace_template(content, pattern, value);
                } else if (key_node->type == YAML_SCALAR_NODE && value_node->type == YAML_SEQUENCE_NODE) {
                    const char *key = (const char *) key_node->data.scalar.value;

                    if (strcmp(key, "rows") != 0)
                        continue;

                    const char *template_row = get_last_w_tr_block(content);
                    if (template_row == NULL)
                        return EXIT_FAILURE;

                    for (const yaml_node_item_t *item = value_node->data.sequence.items.start;
                         item < value_node->data.sequence.items.top;
                         item++) {
                        const yaml_node_t *seq_node = yaml_document_get_node(&doc, *item);
                        if (seq_node == NULL)
                            continue;

                        if (seq_node->type == YAML_MAPPING_NODE) {
                            char *updated_row = copy_string(template_row);

                            for (const yaml_node_pair_t *map_pair = seq_node->data.mapping.pairs.start;
                                 map_pair < seq_node->data.mapping.pairs.top;
                                 map_pair++) {
                                const yaml_node_t *map_key = yaml_document_get_node(&doc, map_pair->key);
                                const yaml_node_t *map_value = yaml_document_get_node(&doc, map_pair->value);

                                if (map_key->type != YAML_SCALAR_NODE && map_value->type != YAML_SCALAR_NODE)
                                    continue;

                                const char *key_str = (const char *) map_key->data.scalar.value;
                                const char *value_str = (const char *) map_value->data.scalar.value;

                                char pattern[512];
                                snprintf(pattern, sizeof(pattern), "{{ %s }}", key_str);
                                updated_row = replace_template(updated_row, pattern, value_str);
                            }
                            const char *replace_value = concat(updated_row, template_row);
                            content = replace_template(content, template_row, replace_value);
                            free(updated_row);
                        }
                    }

                    content = replace_template(content, template_row, "");
                }
            }
        }

        yaml_document_delete(&doc);
    }

    const int write_result = write_file_to_zip(opts->output_path, target_file, content);
    if (write_result != 0) {
        fprintf(stderr, "Error writing file: %s\n", target_file);
        return EXIT_FAILURE;
    }

    free(content);

    return EXIT_SUCCESS;
}
