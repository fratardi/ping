
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
// void fill_icmp_header(struct icmphdr *icp)
// {
//     icp->type = ICMP_ECHO;
//     icp->code = 0;
//     icp->checksum = 0;
//     icp->un.echo.id = htons(getpid());
//     icp->un.echo.id = htons(getpid());
//     //// needs checksum filled 

// 	checksum_packet(icp);
// }


void init_icp_header(struct icmphdr *icp)
{
    (void)memset(icp, 0 ,sizeof(struct icmphdr));
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.sequence =  0 ;
}


struct addrinfo init_hints(struct addrinfo 	*hints)
{
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    return(*hints);
}



// reverse dns lookup from ipv4 to hostname
char *ipv4_to_hostname(char *ipv4)
{
    struct addrinfo 	hints = init_hints(&hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo     *servinfo;
    struct addrinfo 	*p = NULL;
    int rv;
    char hostname[INET_ADDRSTRLEN == 0 ? 1: 256];
    if ((rv = getaddrinfo(ipv4, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }
     for (p = servinfo; p != NULL; p = p->ai_next) {
         void *addr;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        getnameinfo(p->ai_addr, p->ai_addrlen, hostname, sizeof hostname, NULL, 0, NI_NAMEREQD);
         printf("%s\n", hostname);
     }
    freeaddrinfo(servinfo);
    return strdup(hostname);
}





char *hostname_to_ipv6(char *hostname)
{
    struct addrinfo 	hints = init_hints(&hints);
hints.ai_family = AF_INET;

   	struct addrinfo     *servinfo;
	struct addrinfo 	*p = NULL;
    int rv;
    char ip[INET_ADDRSTRLEN == 0 ? 1: 256];
    if ((rv = getaddrinfo(hostname, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        inet_ntop(p->ai_family, addr, ip, sizeof ip);

        printf("%s\n", ip);
    }



    stats.hostname = ipv4_to_hostname(ip);
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


  //  stats.hostinfo = ipv4_to_hostname(argv[1]);
}
