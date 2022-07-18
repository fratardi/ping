
#include "ping.h"

extern struct s_stats stats;


int set_ttl(int sock, int n)
{
	int ret;
	ret = setsockopt(sock, IPPROTO_IP, IP_TTL, &n, sizeof(n));
	if (ret == -1)
		perror("setsockopt");
	return ret;
}



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
	res = result;
	error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
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
	if (!(packet_buffer = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
    icp = (struct icmphdr *)packet_buffer;
	init_icp_header(icp);
	gettimeofday(&stats.timediff.sent,NULL);
	int cc = datalen + 8;

	set_ttl(sock , 255);

	sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));
	printf("Sent %d bytes\n", i);
	memset(&msg, 0, sizeof(msg));
	msg.msg_name = addrbuf;
	msg.msg_namelen = sizeof(addrbuf);
	iov.iov_base = (char *) packet_buffer;
	iov.iov_len = packlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	polling = MSG_WAITALL;
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
