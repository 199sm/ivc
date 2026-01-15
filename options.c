#include "options.h"

char *expand_path(const char *path) {
    if (path == NULL || path[0] != '~') {
        return strdup(path);
    }

    const char *home = getenv("HOME");
    if (home == NULL) {
        home = getenv("USERPROFILE");
    }
    if (home == NULL) {
        return NULL;
    }

    const char *rest = path + 1;
    const size_t len = strlen(home) + strlen(rest) + 1;

    char *expanded = malloc(len);
    if (expanded == NULL) return NULL;

    snprintf(expanded, len, "%s%s", home, rest);
    return expanded;
}

struct options *get_options(const int argc, char *argv[]) {
    struct options *opts = malloc(sizeof(struct options));
    if (opts == NULL) return NULL;
    *opts = (struct options){
        .template_path = expand_path("~/.ivc/template.docx"),
        .config_path = expand_path("~/.ivc/config.yaml"),
        .output_path = expand_path("~/.ivc/invoice.docx"),
        .exit_code = -1
    };

    const struct option long_options[] = {
        {"template", required_argument, 0, 't'},
        {"config", required_argument, 0, 'c'},
        {"output", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int c;
    while ((c = getopt_long(argc, argv, "c:o:vh", long_options, NULL)) != -1) {
        switch (c) {
            case 't':
                opts->template_path = expand_path(optarg);
                break;

            case 'c':
                opts->config_path = expand_path(optarg);
                break;

            case 'o':
                opts->output_path = expand_path(optarg);
                break;

            case 'h':
                printf("Usage: %s [OPTIONS]\n\n", argv[0]);
                printf("Options:\n");
                printf("  -t, --template FILE   Template file path (default: ~/.ivc/template.docx)\n");
                printf("  -c, --config FILE     Config file path (default: ~/.ivc/config.yaml)\n");
                printf("  -o, --output FILE     Output file path (default: ~/.ivc/invoice.docx)\n");
                printf("  -h, --help            Show this help\n");
                opts->exit_code = EXIT_SUCCESS;
                break;

            case '?':
                printf("Unknown option\n");
                opts->exit_code = EXIT_FAILURE;
                break;
        }
    }

    return opts;
}
