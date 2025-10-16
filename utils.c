#include "ping.h"
#include <math.h>

unsigned short calculate_checksum(void *b, int len) {
    unsigned int    sum ;
    unsigned short  *buf;
    unsigned short  ret;
    
    buf = b;
    sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    ret = ~sum;
    return ret;
}

void print_stats(void) {
    double avg_rtt;
    double packet_loss;
    double dev;
    double deviation;
    packet_loss = 0;
    printf("\n--- %s ping statistics ---\n", g_stats.hostname);
    if (g_stats.packets_sent > 0) {
        packet_loss = ((double)(g_stats.packets_sent - g_stats.packets_received) / (double)g_stats.packets_sent) * 100.0;
    }
    printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n", g_stats.packets_sent, g_stats.packets_received, packet_loss);
    if (g_stats.packets_received > 0) {
        avg_rtt = g_stats.sum_rtt / g_stats.packets_received;
        deviation = (g_stats.sum_sq_rtt / g_stats.packets_received) - (avg_rtt * avg_rtt);
        dev = sqrt(deviation > 0 ? deviation : 0);
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",g_stats.min_rtt, avg_rtt, g_stats.max_rtt, dev);
    }
}
