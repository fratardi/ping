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
    g_stats.verbose = 0;
}

void init_packet(int seq, t_ping_packet *packet)
{
    struct timeval  tv;

    memset(packet, 0, sizeof(*packet));
    // setup icmp header
    packet->header.type = ICMP_ECHO;
    packet->header.code = 0;
    packet->header.un.echo.id = getpid() & 0xFFFF;
    packet->header.un.echo.sequence = seq;
    packet->header.checksum = 0;
    // add timestamp to payload
    gettimeofday(&tv, NULL);
    memcpy(packet->msg, &tv, sizeof(tv));
    // fill rest of payload with pattern
    size_t i = sizeof(tv);
    while (i < sizeof(packet->msg)) {
        packet->msg[i] = i;
        i++;
    }
    packet->header.checksum = calculate_checksum(packet, sizeof(*packet));
}
