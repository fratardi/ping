#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef PING_H
#define PING_H

typedef struct s_timediff {
	struct timeval sent;
	struct timeval recieved;
} t_timediff;

typedef struct s_stats {
	char *ip;
	unsigned int total_packets;
	unsigned int success;
	unsigned int failed;
	struct s_timediff timediff;
	struct timeval start;
} t_stats;

/* Function declarations */
/* ping.c */
void handle_sigint(int sig);
void init_stats(int argc, char **argv);
void print_ligne_intermediaire(void);

/* pinger.c */
int pinger(char *str);

/* utils.c */
unsigned short checksum(void *b, int len);
uint16_t checksum_packet(struct icmphdr *icp);
struct timeval get_time_diff(struct timeval start, struct timeval end);
void init_icp_header(struct icmphdr *icp);

/* network.c */
char *hostname_to_ipv6(char *hostname);
int open_icmp_socket(void);
int listen_icmp(int sock);

/* print1.c */
void init_ping(char **argv);

/* Defines */
#define PORT_NO 0
#define MAXIPLEN 60
#define MAXICMPLEN 76
#define PING_PKT_S 64
#define RECV_TIMEOUT 1

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#endif
