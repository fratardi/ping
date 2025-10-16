#include "ping.h"

int parse_args(int argc, char **argv) {
    int i = 1;
    g_stats.ttl = DEFAULT_TTL;
    g_stats.count = 0;      // 0 == inf
    g_stats.interval = 1.0; // Default 1 second
    g_stats.hostname = NULL;
    g_stats.verbose = 0;    // Default not verbose

    while (i < argc) {
        if (strcmp(argv[i], "-v") == 0) {
            g_stats.verbose = 1;
        } else if (strcmp(argv[i], "-?") == 0) {
            print_usage();
        } else if (strcmp(argv[i], "-ttl") == 0) {
            if (i + 1 >= argc) {
                printf("ft_ping: option requires an argument -- 'ttl'\n");
                print_usage();
            }
            i++;
            int ttl = atoi(argv[i]);
            if (ttl < 1 || ttl > 255) {
                printf("ft_ping: invalid TTL value: %s\n", argv[i]);
                exit(1);
            }
            g_stats.ttl = ttl;
        } else if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 >= argc) {
                printf("ft_ping: option requires an argument -- 'c'\n");
                print_usage();
            }
            i++;
            g_stats.count = atoi(argv[i]);
            if (g_stats.count < 1) {
                printf("ft_ping: invalid count value: %s\n", argv[i]);
                exit(1);
            }
        } else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc) {
                printf("ft_ping: option requires an argument -- 'i'\n");
                print_usage();
            }
            i++;
            g_stats.interval = atof(argv[i]);
            if (g_stats.interval < 0.0) {
                printf("ft_ping: invalid interval value: %s\n", argv[i]);
                exit(1);
            }
        } else if (argv[i][0] == '-') {
            printf("ft_ping: invalid option -- '%s'\n", argv[i]);
            print_usage();
        } else {
            if (g_stats.hostname != NULL) {
                printf("ft_ping: too many arguments\n");
                print_usage();
            }
            g_stats.hostname = argv[i];
        }
        i++;
    }

    if (g_stats.hostname == NULL) {
        printf("ft_ping: missing host operand\n");
        print_usage();
    }

    return 0;
}
