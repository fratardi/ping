
#include "ping.h"

extern struct s_stats stats;

struct timeval get_time_diff(struct timeval start, struct timeval end)
{
    struct timeval diff;
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_usec = end.tv_usec - start.tv_usec;
    if (diff.tv_usec < 0) {
        diff.tv_sec--;
        diff.tv_usec += 1000000;
    }
    return diff;
}

/// function that fills the icmpheader icp given as parameter for the ping packet
void fill_icmp_header(struct icmphdr *icp)
{
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.id = htons(getpid());
    icp->un.echo.id = htons(getpid());
    //// needs checksum filled 
}

char *hostname_to_ipv6(char *hostname)
{
    struct addrinfo 	hints;
   	struct addrinfo     *servinfo;
	struct addrinfo 	*p;
    int rv;
    char ip[INET_ADDRSTRLEN == 0 ? 1: 256];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    //hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(hostname, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
      //  if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
      //  }
		//  else {
        //     struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
        //     addr = &(ipv6->sin6_addr);
        //     ipver = "IPv6";
        // }
        inet_ntop(p->ai_family, addr, ip, sizeof ip);
        printf("%s: %s\n", ipver, ip);
    }
    freeaddrinfo(servinfo);
    return strdup(ip);
}

void init_ping( char  **argv )
{
    struct addrinfo* result;
    struct addrinfo* res;
    int error;
    /* resolve the domain name into a list of addresses */
    if ((error =  getaddrinfo(argv[1], NULL, NULL, &result))) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }   
    res = result;
    while( res != NULL) {   
        char hostname[NI_MAXHOST];
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, \
        NI_MAXHOST, NULL, 0, 0); 
        if (error) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname )
            printf("hostname: %s\n", hostname);
        res = res->ai_next;
    }   
}
