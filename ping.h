
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

#ifndef PINH_H
#define PING_H



typedef
struct 		s_timediff	{
 struct 	timeval		sent;
 struct 	timeval		recieved;
}			t_timediff;


typedef
struct s_stats{
	char * ip;
	unsigned int        total_packets;
	unsigned int		success;
	unsigned int		failed;
	struct 				s_timediff	timediff;
	struct timeval			start;
}               t_stats;



// typedef struct  s_stats{
//     int truc;
// }               t_stats;

int pinger(char *str );
void 			handle_sigint(int sig);
void 			print_stats();
void 			init_ping();
char 			*hostname_to_ipv6(char *hostname);
struct timeval 	get_time_diff(struct timeval start, struct timeval end);

uint16_t checksum_packet(struct icmphdr *icp);

void init_icp_header(struct icmphdr *icp);
#define PORT_NO 		0
#define MAXIPLEN  		60
#define MAXICMPLEN 		76
#define PING_PKT_S 		64
#define RECV_TIMEOUT 	1
 //struct timeval trucc;
#endif
// # define CLOCK_MONOTONIC                1
#ifndef   NI_MAXHOST
#define   NI_MAXHOST 1025
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
# define ODDBYTE(v)	(v)
#elif BYTE_ORDER == BIG_ENDIAN
// # define ODDBYTE(v)	((unsigned short)(v) << 8)
// #else
// # define ODDBYTE(v)	htons((unsigned short)(v) << 8)
#endif

// struct addrinfo {
//     int       ai_flags;
//     int       ai_family;
//     int       ai_socktype;
//     int       ai_protocol;
//     socklen_t ai_addrlen;
//     struct    sockaddr* ai_addr;
//     char*     ai_canonname;      /* canonical name */
//     struct    addrinfo* ai_next; /* this struct can form a linked list */
// };

// Icmp header struct 

// struct icmphdr
// {
//   uint8_t type;		/* message type */
//   uint8_t code;		/* type sub-code */
//   uint16_t checksum;
//   union
//   {
//     struct
//     {
//       uint16_t	id;
//       uint16_t	sequence;
//     } echo;			/* echo datagram */
//     uint32_t	gateway;	/* gateway address */
//     struct
//     {
//       uint16_t	__glibc_reserved;
//       uint16_t	mtu;
//     } frag;			/* path mtu discovery */
//   } un;
// };

// ping packet structure

// struct ping_pkt
// {
// 	struct icmphdr hdr;
// 	char msg[PING_PKT_S-sizeof(struct icmphdr)];
// };



/// Domain	
	//  AF_INET      IPv4 Internet protocols         	       	    ip(7)
	//  AF_INET      IPv4 Internet protocols        	            ip(7)
/// Type
	// SOCK_DGRAM UDP datagram socket                   	         udp(7)
/// Protocol
	// IPPROTO_ICMP Internet Control Message Protocol (ICMP)         icmp(7)
	//  int socket(int domain, int type, int protocol);
/// https://stackoverflow.com/questions/5815675/what-is-sock-dgram-and-sock-stream

/// http://www.faqs.org/rfcs/rfc1071.html  for the checksum calculation



		// int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);