#include "ping.h"

extern struct s_stats stats;

char *hostname_to_ipv6(char *hostname)
{
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p = NULL;
	int rv;
	char ip[INET_ADDRSTRLEN == 0 ? 1 : 256];
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
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
	
	freeaddrinfo(servinfo);
	return strdup(ip);
}

int open_icmp_socket(void)
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	
	if (sock == -1) {
		perror("socket");
		return -1;
	}
	
	return sock;
}

int listen_icmp(int sock)
{
#define BUFFER_SIZE 1024
	int ret;
	char buf[BUFFER_SIZE];
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	
	ret = recvfrom(sock, buf, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&addr, &addr_len);
	
	if (ret == -1)
		perror("recvfrom");
	
	return ret;
}
