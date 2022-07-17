#include "ping.h"

struct s_stats stats;


#define __null 0


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
/*
 * @struct timeval get_time_diff(struct timeval start, struct timeval end)
 * @brief calcul de la difference de temps entre deux temps
 * @param start : temps de depart
 * @param end : temps d'arrivee
 * @return struct timeval : difference de temps
* */


uint16_t checksum_packet(struct icmphdr *icp)
{
	uint16_t *addr = (uint16_t *)icp;
	int len = sizeof(struct icmphdr) + sizeof(struct timeval);
	int sum = 0;
	int nleft = len;
	while (nleft > 1) {
		sum += *addr++;
		nleft -= 2;
	}
	if (nleft == 1)
		sum += *(unsigned char *)addr;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (uint16_t)~sum;
}




#include <sys/uio.h>
#include <sys/socket.h>


int pinger(char *str )
{
	int 	error;
	int 	sock = 0 ;
	int		alen = 0;
	int 	datalen = 56;
	int 	ntransmitted = 0;
	int 	i = 0 ;
	int 	polling;
	int 	packlen = datalen + MAXIPLEN + MAXICMPLEN;
	int 	csfailed;

	unsigned char *packet_buffer = NULL;
	char addrbuf[128];
	char hostname[NI_MAXHOST];
	
	struct icmphdr *icp_reply = NULL;
	struct icmphdr *buf = NULL;
	struct icmphdr *icp = NULL;

	struct addrinfo* result = NULL;
	struct addrinfo* res;

	struct sockaddr_in source ;
	struct sockaddr_in dst  ;
	memset((char *)&dst, 	0, 		sizeof(dst));
	memset((char *)&source, 0,	 	sizeof(source));
	source .sin_family = AF_INET ;
		dst.sin_family = AF_INET;


	struct msghdr msg;
	struct iovec iov;

	memset((char *)&iov,  0,sizeof(	struct iovec));


	getaddrinfo(str, NULL, NULL, &result);
	memset(&msg, 0 ,sizeof(struct msghdr));
	res = result;
	error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
	/* We first try to make a UDP connection
	 * on port 1025 to the destination host
	 * so that we can set the source IP correctly
	 */
	/* arv[1] is supposed to be an IP address */
	
//inet_pton(AF_INET, str, &dst.sin_addr);

	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
	dst.sin_port = htons(NI_MAXHOST);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);


	if (sock == -1) {
		perror("Error creating socket");
	}
		// int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	if (getsockname(sock, (struct sockaddr*)&source, (unsigned int *)&alen) == -1) {
			perror("getsockname");
			exit(2);
	}
	/* Now we create the packet that we send down the wire
	 * Since we use IPPROTO_ICMP, we just have to create the
	 * ICMP packet
	 */
	if (!(packet_buffer = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
	icp = (struct icmphdr *)packet_buffer;
	(void)memset(icp, 0 ,sizeof(struct icmphdr));
	/* We are sending a ICMP_ECHO ICMP packet */
	icp->type = ICMP_ECHO;
//icp->type = 16;

	
	icp->code = 0;
	icp->checksum = 0;
	icp->un.echo.sequence = htons(ntransmitted+1);
	/* We don't set the echo.id here since IPPROTO_ICMP does it for us
	 * it sets it to the source port
	 * pfh.icmph.un.echo.id = inet->inet_sport;
	 */
	/* compute ICMP checksum here */
	int cc = datalen + 8;
//	icp->checksum = checksum((unsigned short *)icp, cc, 0);

	checksum_packet(icp);

	/* send the ICMP packet*/
	gettimeofday(&stats.timediff.sent,NULL);



	sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));
	printf("Sent %d bytes\n", i);
	/* We have sent the packet, time to attempt to read
	 * the reply
	 * */


	printf(  " packlen -==  [%d]\n" , packlen);  
//	printf(  " packet -==  [%s]\n" ,packet);  


	memset(&msg, 0, sizeof(msg));
	/* check recvmsg() to understand the reasoning/meaning
	 * for the different fields
	 */
	msg.msg_name = addrbuf;
	msg.msg_namelen = sizeof(addrbuf);
	/* Learn more: 
	 * https://www.safaribooksonline.com/library/view/linux-system-programming/9781449341527/ch04.html
	 */

	iov.iov_base = (char *) packet_buffer;
	iov.iov_len = packlen;
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
	csfailed = checksum_packet(icp_reply);
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

 printf("Reply type %d", icp_reply->type);


	if (icp_reply->type == ICMP_ECHOREPLY) {
		  // printf("%s\n", pr_addr(from, sizeof *from));
		   printf("Reply of %d bytes received ", cc);
		   printf("icmp_seq = %u\n", ntohs(icp_reply->un.echo.sequence));
	} else {
		printf("Not a ICMP_ECHOREPLY\n");
	}
	free(packet_buffer);
	freeaddrinfo(result);
	printf("size%lu" , sizeof( socklen_t) * 8);
	return 0;
}

/*
 * Checksum routine for ICMP packets.
 * */

void init_stats(int argc , char **argv)
{
	(void)argc,
	(void)argv;
	stats.ip = hostname_to_ipv6(argv[1]);
	stats.success 				= 0,
	stats.total_packets 		= 0,
	stats.failed 				= 0,
	gettimeofday(&stats.start, NULL);

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
	signal(SIGINT, handle_sigint);
	init_stats(argc,  argv);
	printf( "IP = [%s]" , stats.ip );
	if(stats.ip)
	while(1)
	{
		pinger(stats.ip);
		print_ligne_intermediaire();
		sleep(1);
	}
	return(0);
}


//