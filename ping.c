#include "ping.h"

struct s_stats stats;

void handle_sigint(int sig)
{  
	(void)sig;
	struct timeval end;
	gettimeofday(&end,NULL);
	unsigned int duration_secondes  =   +end.tv_sec - stats.start.tv_sec  ;
	int duration_usec =   ((end.tv_sec - stats.start.tv_sec ) /1000) > 100    ? stats.start.tv_usec -  end.tv_usec   : 
  		end.tv_usec - stats.start.tv_usec  ; 
	printf("time  %u.%ums\n" , duration_secondes, duration_usec / 1000);
	free (stats.ip);
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




int pinger(char *str )
{
	int error;
	int sock = 0 ;
	int  alen = 0;
	int datalen = 56;
	int ntransmitted = 0;
	int i = 0 ;
	int polling;
	int packlen = datalen + MAXIPLEN + MAXICMPLEN;
	int csfailed;
	unsigned char *packet = NULL;
	char addrbuf[128];
	char hostname[NI_MAXHOST];
	struct icmphdr *icp_reply = NULL;
	struct icmphdr *buf = NULL;
	struct icmphdr *icp = NULL;
	struct addrinfo* result = NULL;
	struct addrinfo* res;
	struct sockaddr_in source = { .sin_family = AF_INET };
	struct sockaddr_in dst;
	struct msghdr msg;
	struct iovec iov;

	getaddrinfo(str, NULL, NULL, &result);
	memset(&msg, 0 ,sizeof(struct msghdr));
	res = result;
	error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
	/* We first try to make a UDP connection
	 * on port 1025 to the destination host
	 * so that we can set the source IP correctly
	 */
	memset((char *)&dst, 0, sizeof(dst));
	dst.sin_family = AF_INET;
	/* arv[1] is supposed to be an IP address */
	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
	dst.sin_port = htons(NI_MAXHOST);
	// Create a socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (sock == -1) {
		perror("Error creating socket");
	}
	if (getsockname(sock, (struct sockaddr*)&source, (unsigned int *)&alen) == -1) {
			perror("getsockname");
			exit(2);
	}
	/* Now we create the packet that we send down the wire
	 * Since we use IPPROTO_ICMP, we just have to create the
	 * ICMP packet
	 */
	if (!(packet = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
	icp = (struct icmphdr *)packet;
	(void)memset(icp, 0 ,sizeof(struct icmphdr));
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
	gettimeofday(&stats.timediff.sent,NULL);
	sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));
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
	gettimeofday(&stats.timediff.recieved,NULL);
	printf(" message.namelen = [%u]\n", msg.msg_namelen);
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
	// struct sockaddr_in *from = msg.msg_name;
	if (icp_reply->type == ICMP_ECHOREPLY) {
		  // printf("%s\n", pr_addr(from, sizeof *from));
		   printf("Reply of %d bytes received ", cc);
		   printf("icmp_seq = %u\n", ntohs(icp_reply->un.echo.sequence));
	} else {
		printf("Not a ICMP_ECHOREPLY\n");
	}
	free(packet);
	freeaddrinfo(result);
	printf("size%lu" , sizeof( socklen_t) * 8);
	return 0;
}

void init_stats()
{
	stats.success 				= 0;
	stats.total_packets 		= 0;
	stats.failed 				= 0;
	gettimeofday(&stats.start,NULL);

}

void  print_ligne_intermediaire(void)
{
	suseconds_t diff =  (stats.timediff.recieved.tv_usec-  stats.timediff.sent.tv_usec)  /10;
	stats.total_packets += 1;
	size_t len = sizeof(	struct icmphdr);
	printf("\n [%zu ]icmp.seq=%u time=%lu.%lu ms\n" ,len , stats.total_packets  ,diff / 100 ,   diff % 100);
}


int main(int argc , char **argv)
{
	(void)argc;
	int sockfd;
	signal(SIGINT, handle_sigint);
	stats.ip = hostname_to_ipv6(argv[1]);
	init_stats();
	printf( "IP = [%s]" , stats.ip );
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(stats.ip)
	while(1)
	{
		pinger(stats.ip);
		print_ligne_intermediaire();
		sleep(1);
	}
	return(0);
}


