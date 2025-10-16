#include "ping.h"

t_ping_stats g_stats;
volatile sig_atomic_t g_running = 1;

void signal_handler(int sig) {
    (void)sig;
    g_running = 0;
}

void print_usage(void) {
    fprintf(stderr, "Usage: ./ping [-c count] [-i interval] [-ttl N] destination\n");
    fprintf(stderr, "  -c count      Stop after sending count packets\n");
    fprintf(stderr, "  -i interval   Wait interval seconds between sending each packet (default: 1.0)\n");
    fprintf(stderr, "  -ttl N        Set Time To Live (1-255)\n");
    exit(1);
}


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

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in addr;
    int seq = 0;

    if (geteuid() != 0) {
        fprintf(stderr, "ft_ping: Lacking privilege for raw socket.\n");
        return 1;
    }

    init_g_stats(argc,  argv);
    parse_args(argc, argv);
    // Host res 
    g_stats.ip_addr = resolve_hostname(g_stats.hostname);
    if (g_stats.ip_addr == NULL) {
        fprintf(stderr, "ft_ping: unknown host %s\n", g_stats.hostname);
        return 1;
    }
    // init sock
    sockfd = create_icmp_socket();
    if (sockfd < 0) {
        return 1;
    }

    // Set TTL
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &g_stats.ttl, sizeof(g_stats.ttl)) != 0) {
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    // Setup destination address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(g_stats.ip_addr);

    // Setup signal handler
    signal(SIGINT, signal_handler);

    // Print start message
    printf("PING %s (%s): %d data bytes\n", 
           g_stats.hostname, g_stats.ip_addr, PACKET_SIZE - 8);

    gettimeofday(&g_stats.start_time, NULL);


    while (g_running && (g_stats.count == 0 || seq < g_stats.count)) {
        send_ping(sockfd, &addr, seq);
        if (receive_ping(sockfd, seq) == 0) {
            g_stats.packets_received++;
        }
        seq++;

        if (g_stats.count == 0 || seq < g_stats.count) // if first pack no sleep
        {
            usleep((unsigned int)(g_stats.interval * 1000000));
        }
    }

    close(sockfd);
    print_statistics();

    return 0;
}
