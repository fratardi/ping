#include "ping.h"

#define PING_PKT_S 64
  
// Automatic port number
#define PORT_NO 0
 
// Automatic port number
// #define PING_SLEEP_RATE 1000000 x
 
// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1
 
// Define the Ping Loop
int pingloop=1;
 
 
// ping packet structure
struct ping_pkt
{
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

void handle_sigint(int sig)
{  


    print_stats();
  //   printf("HEllo%d",trucc);



    printf("Caught signal %d\n", sig);
      exit(EXIT_SUCCESS);
}


uint16_t checksum(const unsigned short *addr, register int len, unsigned short csum)
{
	register int nleft = len;
	const unsigned short *word = addr;
	register uint16_t  answer;
	register int sum = csum;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while (nleft > 1)  {
		sum += *word++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1)
		sum += ODDBYTE(*(unsigned char *)word); /* le16toh() may be unavailable on old systems */

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}




int pinger(char **argv )
{

struct icmphdr *buf = NULL;
    struct addrinfo* result;
    struct addrinfo* res;
    int error;


    struct icmphdr *icp_reply = NULL;

    int sock = 0 ;
    int  alen = 0;
    struct sockaddr_in source = { .sin_family = AF_INET };
    struct sockaddr_in dst;

    int datalen = 56;
    int MAXIPLEN = 60;
    int MAXICMPLEN = 76;
    unsigned char *packet = NULL;
    struct icmphdr *icp = NULL;
    int ntransmitted = 0;

    struct msghdr msg;
    int polling;
    char addrbuf[128];
    struct iovec iov;
int i = 0 ;
    int packlen = datalen + MAXIPLEN + MAXICMPLEN;
        int csfailed;

    getaddrinfo(argv[1], NULL, NULL, &result);
    res = result;
         char hostname[NI_MAXHOST];
    error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 



    /* We first try to make a UDP connection
     * on port 1025 to the destination host
     * so that we can set the source IP correctly
     */
    memset((char *)&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    /* arv[1] is supposed to be an IP address */
    if (inet_aton(argv[1], &dst.sin_addr) == 0) {
        fprintf(stderr, "The first argument must be an IP address\n");
        exit(1);
    }
    dst.sin_port = htons(1025);
    // Create a socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (sock == -1) {
        perror("Error creating socket");
    }
    if (getsockname(sock, (struct sockaddr*)&source, &alen) == -1) {
			perror("getsockname");
			exit(2);
	}
    // Source IP address that's set
    //char *ip = inet_ntoa(source.sin_addr);
    //printf("%s\n", ip);
    
    /* Now we create the packet that we send down the wire
     * Since we use IPPROTO_ICMP, we just have to create the
     * ICMP packet
     */


	if (!(packet = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
    icp = (struct icmphdr *)packet;
    /* We are sending a ICMP_ECHO ICMP packet */
	icp->type = ICMP_ECHO;
	icp->code = 0;
	icp->checksum = 0;
	icp->un.echo.sequence = htons(ntransmitted+1);
    /* We don't set the echo.id here since IPPROTO_ICMP does it for us
     * it sets it to the source port
     * pfh.icmph.un.echo.id = inet->inet_sport;
     */

    /* compute ICMP checksum here */
    int cc = datalen + 8;
	icp->checksum = checksum((unsigned short *)icp, cc, 0);

    /* send the ICMP packet*/
    i = sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));
    printf("Sent %d bytes\n", i);

    /* We have sent the packet, time to attempt to read
     * the reply
     * */


    iov.iov_base = (char *) packet;
    iov.iov_len = packlen;

    memset(&msg, 0, sizeof(msg));

    /* check recvmsg() to understand the reasoning/meaning
     * for the different fields
     */
    msg.msg_name = addrbuf;
    msg.msg_namelen = sizeof(addrbuf);
    /* Learn more: 
     * https://www.safaribooksonline.com/library/view/linux-system-programming/9781449341527/ch04.html
     */
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    /* We do a blocking wait here */
    polling = MSG_WAITALL;
    
    /* check the man page for recvmsg to understand why we need
     * to pass msg here
     * TLDR: passing msg here allows us to check the source
     * address for the unconnected socket, sock
    */
    cc = recvmsg(sock, &msg, polling);
    printf(" message.namelen = [%u]\n", msg.msg_namelen);


    printf(" message = [%u]", msg.msg_control);



    if (cc  < 0 ){
        perror("Error in recvmsg");
        exit(1);
    }
  buf = msg.msg_iov->iov_base;

    icp_reply = (struct icmphdr *)buf;
    csfailed = checksum((unsigned short *)icp_reply, cc, 0);
    if (csfailed) {
        printf("(BAD CHECKSUM)");
        exit(1);
    }

    /* Note that we don't have to check the reply ID to match that whether
     * the reply is for us or not, since we are using IPPROTO_ICMP.
     * See https://lwn.net/Articles/443051/ ping_v4_lookup()
     * If we were using a RAW socket, we would need to do that.
     * */
    struct sockaddr_in *from = msg.msg_name;
    if (icp_reply->type == ICMP_ECHOREPLY) {
          // printf("%s\n", pr_addr(from, sizeof *from));
           printf("Reply of %d bytes received ", cc);
           printf("icmp_seq = %u\n", ntohs(icp_reply->un.echo.sequence));
    } else {
        printf("Not a ICMP_ECHOREPLY\n");
    }
    return 0;

}


// prints all the values containes in the sockaddr_in structure
// and returns a string representation of the address
char *pr_addr(struct sockaddr *sa, socklen_t salen)
{
    static char str[128];
    char *ptr;
    int i;
    struct sockaddr_in *sin = (struct sockaddr_in *)sa;

    if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
        return NULL;
    ptr = strchr(str, '\0');
    snprintf(ptr, sizeof(str) - (ptr - str), ":%d", ntohs(sin->sin_port));
    return str;
}



int main(int argc , char **argv)
{
    int sockfd;
  //  struct sockaddr_in addr_con;
//    char *ip_addr = NULL;
  //  char  *reverse_hostname= NULL;

    signal(SIGINT, handle_sigint);

    // if(argc != 0)

printf("[%s]", hostname_to_ipv6(argv[1]));
 //  init_ping(   argv );

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);


    while(1)
    {
        pinger(argv);
        //send pings continuously
     //   send_ping(argv);
        sleep(1);
    }
  //freeaddrinfo(result);


    return 0;
}


