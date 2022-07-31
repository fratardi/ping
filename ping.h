
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
#include <sys/uio.h>
#include <sys/socket.h>

#include <pthread.h>

#ifndef PINH_H
#define PING_H
#define BUFFER_SIZE 1024


// typedef
// struct s_list{
// 	void * memory;
// 	size_t size;
// 	t_list *next;
// }		t_list;


typedef
struct 		s_timediff	{
 struct 	timeval		sent;
 struct 	timeval		recieved;
}			t_timediff;



typedef
struct s_packet_infos{

	struct 	icmphdr		icmp_header;
	struct 	sockaddr_in	dest_addr;
	struct 	sockaddr_in	src_addr;
	struct 	sockaddr_in	from_addr;
	size_t packet_size;
	char ttl;

	struct msghdr msg;
	struct iovec iov;

	struct in6_addr *addrv6;
	struct in_addr 	*addrv4;
	struct icmphdr *buf;
	struct sockaddr_in source ;
	struct sockaddr_in dst  ;
}			t_packet_infos;


// grosse valise globale

typedef
struct s_stats{
	struct addrinfo* hostinfo;

	unsigned char * packet_buffer;	
char *addrbuf; 
	int packlen;
/// utils 
	int fd ; 

/// packet infos
	struct s_packet_infos packet_infos;
	char 	* arg ;
	char 	* ip;	
	char 	* hostname;
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
char *ipv4_to_hostname(char *ipv4);
unsigned  char only_one_valid_place(int argc , char** argv);
unsigned int argvparse(int argc , char** argv);
uint16_t checksum_packet(struct icmphdr *icp);

void		print_ligne_intermediaire();;
void init_icp_header(struct icmphdr *icp);
#define PORT_NO 		0
#define MAXIPLEN  		60
#define MAXICMPLEN 		76
#define PING_PKT_S 		56
#define RECV_TIMEOUT 	1
#define OPTS 			"l46v"

#define __null 0
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

// struct sockaddr {
// sa_family_t  sa_family  Address family. 
// char         sa_data[]  Socket address (variable-length data)
//};


// struct in6_addr
//   {
//     union
//       {
// 	uint8_t	__u6_addr8[16];
// 	uint16_t __u6_addr16[8];
// 	uint32_t __u6_addr32[4];
//       } __in6_u;

//   };


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

// https://www.geeksforgeeks.org/ping-in-c/

// set

// #define IP_OPTIONS          1           /* set/get IP per-packet options    */
// #define IP_MULTICAST_IF     2           /* set/get IP multicast interface   */
// #define IP_MULTICAST_TTL    3           /* set/get IP multicast timetolive  */
// #define IP_MULTICAST_LOOP   4           /* set/get IP multicast loopback    */
// #define IP_ADD_MEMBERSHIP   5           /* add  an IP group membership      */
// #define IP_DROP_MEMBERSHIP  6           /* drop an IP group membership      */
// #define IP_TTL              7           /* set/get IP Time To Live          */
// #define IP_TOS              8           /* set/get IP Type Of Service       */
// #define IP_DONTFRAGMENT     9           /* set/get IP Don't Fragment flag   */
// #define IP_DEFAULT_MULTICAST_TTL   1    /* normally limit m'casts to 1 hop  */
// #define IP_DEFAULT_MULTICAST_LOOP  1    /* normally hear sends if a member  */
// #define IP_MAX_MEMBERSHIPS         20   /* per socket; must fit in one mbuf */
// https://www.frameip.com/entete-icmp/
// https://inetdoc.developpez.com/tutoriels/filtrage-iptables/?page=page_3