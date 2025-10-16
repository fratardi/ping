#include "ping.h"

extern struct s_stats stats;

unsigned short checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

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

void init_icp_header(struct icmphdr *icp)
{
	(void)memset(icp, 0, sizeof(struct icmphdr));
	icp->type = ICMP_ECHO;
	icp->code = 0;
	icp->checksum = 0;
	icp->un.echo.sequence = 0;
}
