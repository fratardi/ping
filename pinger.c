
#include "ping.h"

extern struct s_stats stats;




/// 

// IPPROTO_IP
//     Internet protocol.
// IPPROTO_IPV6
//     [IP6] [Option Start] Internet Protocol Version 6. [Option End]
int set_ttl(int sock, int n)
{
	int ret;
	ret = setsockopt(sock, IPPROTO_ICMP	, IP_TTL, &n, sizeof(n));
	if (ret == -1)
		perror("setsockopt");
	return ret;
}

// print sockaddr_in struct	
void print_sockaddr_in(struct sockaddr_in *addr)
{
	printf("sin_family: %d\n", addr->sin_family);
	printf("sin_port: %d\n", addr->sin_port);
	printf("sin_addr: %s\n", inet_ntoa(addr->sin_addr));
}

// print in6_addr struct
void print_in6_addr(struct in6_addr *addr)
{
	int i;
	for (i = 0; i < 16; i++)
		printf("%02x", addr->s6_addr[i]);
	printf("\n");
}

// print sockaddr_in6 struct
void print_sockaddr_in6(struct sockaddr_in6 *addr)
{

	printf("sin6_port: %d\n", addr->sin6_port);
	printf("sin6_flowinfo: %d\n", addr->sin6_flowinfo);
	print_in6_addr(&addr->sin6_addr);
	printf("sin6_scope_id: %d\n", addr->sin6_scope_id);
}

// print icmp struct
void print_icmp(struct icmp *icmp)
{
	printf("icmp_type: %d\n", icmp->icmp_type);
	printf("icmp_code: %d\n", icmp->icmp_code);
	printf("icmp_cksum: %d\n", icmp->icmp_cksum);
	printf("icmp_id: %d\n", icmp->icmp_id);
	printf("icmp_seq: %d\n", icmp->icmp_seq);
}



//hostname to ipv4 address
int hostname_to_ipv4(char *hostname, struct in_addr *addr)
{
	struct hostent *h;
	h = gethostbyname(hostname);
	if (h == NULL)
		return -1;
	addr->s_addr = *(long *)h->h_addr;
	return 0;
}

//hostname to ipv6 address
int hostname_to_ip6(char *hostname, struct in6_addr *addr)
{
	struct hostent *h;
	h = gethostbyname2(hostname, AF_INET6);
	if (h == NULL)
		return -1;
	memcpy(addr, h->h_addr, h->h_length);
	return 0;
}

int pinger(char *str )
{

	printf("dom == [%s]", stats.hostname);


	//int 	error;
	int 	sock = 0 ;
	int		alen = 0;
	int 	datalen = 56;
	int 	i = 0 ;
	
	int 	packlen = datalen + MAXIPLEN + MAXICMPLEN;

	unsigned char *packet_buffer = NULL;
	char addrbuf    [INET6_ADDRSTRLEN] ; //message you want to send in ? 
	char hostname[NI_MAXHOST];
	
	struct icmphdr *icp_reply = NULL;
	struct icmphdr *buf = NULL;
	struct icmphdr *icp = NULL;

	struct addrinfo* result = NULL;
	struct addrinfo* res;




// struct sockaddr_in6 *sin6 = null;//
// 	sin6->sin6_family = AF_INET6;

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


/// fits all the data about the host in the result struct
	getaddrinfo(str, NULL, NULL, &result);	



	res = result;

/// get info about the host 
//	error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
	printf("hostname: %s\n res->ai_addr\n", hostname);


	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
printf("1\n");

	if (sock == -1) {
		perror("Error creating socket");
	}
	if (getsockname(sock, (struct sockaddr*)&source, (unsigned int *)&alen) == -1) 
	{
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
	set_ttl(sock ,255);
	//sizetosend(packlen);
	size_t  mch  =   sendto(sock, icp, cc, 0, (struct sockaddr*)&dst, sizeof(dst));



	printf("mch == %zu   cc  == %d \n", mch, cc);



	printf("Sent %d bytes", i);
	memset(&msg, 0, sizeof(msg));
	msg.msg_name = addrbuf;
	msg.msg_namelen = sizeof(addrbuf);
	iov.iov_base = (char *) packet_buffer;
	iov.iov_len = packlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	cc = recvmsg(sock, &msg, MSG_WAITALL);


	printf("  \n\n%x\n",  msg.msg_flags );




	gettimeofday(&stats.timediff.recieved,NULL);
	printf(" message.namelen = [%u]\n", msg.msg_namelen);


printf ("%d\n", msg.msg_namelen);
write( 1, &msg.msg_name ,msg.msg_namelen );  
write( 1, "\n" ,1 );
printf ("%zu\n", msg.msg_controllen);
write( 1, &msg.msg_control ,msg.msg_controllen );
write( 1, "\n" ,1 );
	if (cc  < 0 ){
		perror("Error in recvmsg");
		exit(1);
	}
	buf = msg.msg_iov->iov_base;
	icp_reply = (struct icmphdr *)buf;
	// if (!checksum_packet(icp_reply)) {
	// 	printf("(BAD CHECKSUM)");
	// 	exit(1);
	// }

	printf(  "   replycode = %d" ,  icp_reply->code   );



	if (icp_reply->type == ICMP_ECHOREPLY) {
		  // printf("%s\n", pr_addr(from, sizeof *from));
		   printf("Reply of %d bytes received ", cc);
		   printf("icmp_seq = %u\n", ntohs(icp_reply->un.echo.sequence));
	} else {
		printf("Not a ICMP_ECHOREPLY\n");
	}


		print_ligne_intermediaire();

	

	// print_sockaddr_in(&source);
		

	// print_sockaddr_in(&dst);



	free(packet_buffer);
	freeaddrinfo(result);
	printf("size%lu" , sizeof( socklen_t) * 8);
	return 0;
}
