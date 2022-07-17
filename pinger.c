
#include "ping.h"

extern struct s_stats stats;


int pinger(char *str )
{
	int 	error;
	int 	sock = 0 ;
	int		alen = 0;
	int 	datalen = 56;
	int 	i = 0 ;
	int 	polling;
	int 	packlen = datalen + MAXIPLEN + MAXICMPLEN;

	unsigned char *packet_buffer = NULL;
	char addrbuf[INET6_ADDRSTRLEN];
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
	source 	.sin_family = AF_INET ;
	dst		.sin_family = AF_INET;
	dst		.sin_port 	= htons(NI_MAXHOST);

	struct msghdr msg;
	struct iovec iov;

	memset((char *)&iov,  0,sizeof(	struct iovec));
	memset(&msg, 0 ,sizeof(struct msghdr));

	getaddrinfo(str, NULL, NULL, &result);
	 //getaddrinfo(hostname, NULL, &hints, &servinfo)
	res = result;


	error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
	/* We first try to make a UDP connection
	 * on port 1025 to the destination host
	 * so that we can set the source IP correctly
	 */
	/* arv[1] is supposed to be an IP address */
	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
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
	if (!(packet_buffer = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
	icp = (struct icmphdr *)packet_buffer;

	/* We are sending a ICMP_ECHO ICMP packet */
	init_icp_header(icp);


	/* We don't set the echo.id here since IPPROTO_ICMP does it for us
	 * it sets it to the source port
	 * pfh.icmph.un.echo.id = inet->inet_sport;
	 */
	/* compute ICMP checksum here */



	/* send the ICMP packet*/
	gettimeofday(&stats.timediff.sent,NULL);


	int cc = datalen + 8;
	sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));
	printf("Sent %d bytes\n", i);
	/* We have sent the packet, time to attempt to read
	 * the reply
	 * */

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
	if (checksum_packet(icp_reply)) {
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
