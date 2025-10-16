#include "ping.h"
#include <math.h>

unsigned short calculate_checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    
    if (len == 1)
        sum += *(unsigned char *)buf;
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    
    return result;
}

void print_statistics(void) {
    double avg_rtt, stddev;
    double packet_loss;

    printf("\n--- %s ping statistics ---\n", g_stats.hostname);
    
    packet_loss = 0.0;
    if (g_stats.packets_sent > 0) {
        packet_loss = ((double)(g_stats.packets_sent - g_stats.packets_received) / 
                      (double)g_stats.packets_sent) * 100.0;
    }

    printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n",
           g_stats.packets_sent, g_stats.packets_received, packet_loss);

    if (g_stats.packets_received > 0) {
        avg_rtt = g_stats.sum_rtt / g_stats.packets_received;
        
        // Calculate standard deviation
        double variance = (g_stats.sum_sq_rtt / g_stats.packets_received) - (avg_rtt * avg_rtt);
        stddev = sqrt(variance > 0 ? variance : 0);

        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
               g_stats.min_rtt, avg_rtt, g_stats.max_rtt, stddev);
    }
}
