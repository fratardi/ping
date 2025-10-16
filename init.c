#include "ping.h"


void init_g_stats(int argc, char **argv )
{
    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.hostname = argv[argc - 1];
    g_stats.min_rtt = 999999.0;
    g_stats.max_rtt = 0.0;
    g_stats.sum_rtt = 0.0;
    g_stats.sum_sq_rtt = 0.0;
    g_stats.packets_sent = 0;
    g_stats.packets_received = 0;
}


