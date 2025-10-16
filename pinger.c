#include "ping.h"

extern struct s_stats stats;

int pinger(char *str)
{
	int sock = 0;
	int alen = 0;
	unsigned char *packet_buffer = NULL;
	char addrbuf[INET6_ADDRSTRLEN] = "helloworld\n\0";
	struct icmphdr *icp = NULL;
	struct addrinfo* result = NULL;
	struct sockaddr_in source;
	struct sockaddr_in dst;
	size_t datalen = strlen(addrbuf);
	int packlen = datalen + sizeof(struct icmphdr);
	int ttl = 64;
	
	source.sin_family = AF_INET;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(NI_MAXHOST);
	
	getaddrinfo(str, NULL, NULL, &result);
	
	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
	
	sock = open_icmp_socket();
	if (sock == -1) {
		perror("Error creating socket");
		exit(1);
	}
	
	if (getsockname(sock, (struct sockaddr*)&source, (unsigned int *)&alen) == -1) {
		perror("getsockname");
		exit(2);
	}
	
	if (!(packet_buffer = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}
	bzero(packet_buffer, packlen);
	
	icp = (struct icmphdr *)packet_buffer;
	init_icp_header(icp);
	icp->checksum = checksum(icp, packlen);
	
	gettimeofday(&stats.timediff.sent, NULL);
	
	int cc = datalen + 8;
	struct timeval tv_out;
	tv_out.tv_sec = 0;
	tv_out.tv_usec = 0;
	
	setsockopt(sock, SOL_IP, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);
	setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	
	size_t mch = sendto(sock, icp, cc, 8, (struct sockaddr*)&dst, sizeof(dst));
	if (!mch) {
		perror("sendto");
		exit(2);
	}
	
	listen_icmp(sock);
	
	gettimeofday(&stats.timediff.recieved, NULL);
	free(packet_buffer);
	close(sock);
	
	return 1;
}
