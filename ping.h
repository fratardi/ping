#ifndef PING_H
#define PING_H

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define PACKET_SIZE 64
#define RECV_TIMEOUT 1
#define DEFAULT_TTL 64

typedef struct s_ping_packet {
        struct icmphdr  header;
        char            msg[PACKET_SIZE - sizeof(struct icmphdr)];
}              t_ping_packet;

typedef struct      s_ping_stats {
    char            *hostname;
    char            *ip_addr;
    // stats 
    unsigned int    packets_sent;
    unsigned int    packets_received;
    double          min_rtt;             // return time stats  
    double          max_rtt;             //
    double          sum_rtt;             //
    double          sum_sq_rtt;          //
    struct timeval  start_time;         //def @ init
    int             ttl;                    // ttl
    int             count;                  //num of packet (0 == infinite)
    double          interval;            // Delay between packets in seconds
}                   t_ping_stats;

extern          t_ping_stats g_stats;
extern volatile sig_atomic_t g_running;

void            signal_handler(int sig);
void            print_usage(void);
int             parse_args(int argc, char **argv);
char            *resolve_hostname(const char *hostname);
int             create_icmp_socket(void);
void            send_ping(int sockfd, struct sockaddr_in *addr, int seq);
int             receive_ping(int sockfd, int seq);
unsigned short  calculate_checksum(void *b, int len);
void            print_stats(void);
void            init_g_stats(int argc, char **argv);

#endif
