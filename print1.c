
#include "ping.h"

//   struct timeval {
//    long tv_sec;    /* seconds */
//    long tv_usec;   /* microseconds */
// };

// in_cksum() and in_cksum_shouldbe() are used to check the validity of the
// checksum field in the IP header.





/// function that fills the icmpheader icp given as parameter for the ping packet
void fill_icmp_header(struct icmphdr *icp)
{
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.id = htons(getpid());
    // icp->checksum = in_cksum((unsigned short *)icp, sizeof(struct icmphdr));

    icp->un.echo.id = htons(getpid());
}






char *hostname_to_ipv6(char *hostname)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char ip[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(hostname, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        inet_ntop(p->ai_family, addr, ip, sizeof ip);
        printf("%s: %s\n", ipver, ip);
    }
    freeaddrinfo(servinfo);
    return ip;
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
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
        if (error) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname )
            printf("hostname: %s\n", hostname);
        res = res->ai_next;
    }   





}


void print_stats()
{}