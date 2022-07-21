

#include "ping.h"

extern struct s_stats stats;
// print binary representation of given ipv6 address
void print_ipv6(struct in6_addr ip)
{
    int i;
    for (i = 0; i < 16; i++)
        printf("%02x", ip.s6_addr[i]);
    printf("\n");
}


// hostname to ipv4 address

// print binary representation of given ipv4 address
void print_ipv4(struct in_addr ip)
{
    int i;
    for (i = 0; i < 4; i++)
        printf("%d", ip.s_addr >> (i * 8) & 0xff);
    printf("\n");
}

int set_ttl(int sock, int n)
{
	int ret;
	ret = setsockopt(sock, IPPROTO_IP, IP_TTL, &n, sizeof(n));
	if (ret == -1)
		perror("setsockopt");
	return ret;
}


// check if given argument is a valid IPv4 address
int is_valid_ipv4(char *str)
{
    struct in_addr addr;
    return inet_pton(AF_INET, str, &addr);
}

// check if given argument is a valid IPv6 address
int is_valid_ipv6(char *str)
{
    struct in6_addr addr;
   // return 
    
    inet_pton(AF_INET6, str, &addr);

// print binary representation of given ipv6 address
    print_ipv6(addr);

    return(  inet_pton(AF_INET6, str, &addr));
}


// check if argument given is a domain name
int is_domain_name(char *str)
{
    struct addrinfo hints;
    struct addrinfo *result;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(str, NULL, &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 0;
    }
    freeaddrinfo(result);
    return 1;
}



// void init_stats(int argc , char **argv)

/// function that fills the icmpheader icp given as parameter for the ping packet
void fill_icmp_header(struct icmphdr *icp)
{
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.id = htons(getpid());
    icp->un.echo.id = htons(getpid());
    //// needs checksum filled in later
	checksum_packet(icp);
}

void init_icp_header(struct icmphdr *icp)
{
    (void)memset(icp, 0 ,sizeof(struct icmphdr));
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.sequence =  0 ;
}

struct addrinfo init_hints(struct addrinfo 	*hints  )
{
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    return(*hints);
}


void init_stats(int argc , char **argv)
{
	(void)argc,
	(void)argv;

	stats.success 				= 0,
	stats.total_packets 		= 0,
	stats.failed 				= 0,
	gettimeofday(&stats.start, NULL);


    if(is_valid_ipv4(argv[1]))
    {
         printf("%s is a valid IPv4 address\n", argv[1]);
	   stats.ip = hostname_to_ipv6(argv[1]);

    }   
    if(is_valid_ipv6(argv[1]))
    {
      stats.ip = hostname_to_ipv6(argv[1] );
        printf("%s is a valid IPv6 address\n", argv[1]);
    }    
    if(is_domain_name(argv[1]))
    {
        printf("%s is a domain name\n", argv[1]);
        stats.ip = hostname_to_ipv6(argv[1]);
        printf(  "  %s  %s", argv[1],  stats.ip);

    }

}

