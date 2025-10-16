#include "ping.h"

int create_icmp_socket(void) {
	int sockfd;
	struct timeval tv_out;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		printf("%s","socket");
		return -1;
	}
	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0) {
		printf("%s","setsockopt");
		close(sockfd);
		return -1;
	}
	return sockfd;
}

void send_ping(int sockfd, struct sockaddr_in *addr, int seq) {
	t_ping_packet   packet;
	int             ret;

	init_packet(seq, &packet);
	ret = sendto(sockfd, &packet, sizeof(packet), 0,
				 (struct sockaddr *)addr, sizeof(*addr));
	if (ret < 0) {
		printf("%s","sendto");
	} else {
		g_stats.packets_sent++;
	}
}


int process_valid_icmp_echo_reply(char *buffer,int ip_header_len, int ret, int seq, struct timeval tv_recv,  struct iphdr *ip_hdr)
{
	struct timeval tv_sent;
	double rtt;

	memcpy(&tv_sent, buffer + ip_header_len + sizeof(struct icmphdr), sizeof(tv_sent));
	// Rtt value  
	rtt = (tv_recv.tv_sec - tv_sent.tv_sec) * 1000.0 +
		  (tv_recv.tv_usec - tv_sent.tv_usec) / 1000.0;
	// stat Update 
	if (rtt < g_stats.min_rtt) g_stats.min_rtt = rtt;
	if (rtt > g_stats.max_rtt) g_stats.max_rtt = rtt;
	g_stats.sum_rtt += rtt;
	g_stats.sum_sq_rtt += rtt * rtt;
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",ret - ip_header_len, g_stats.ip_addr, seq, ip_hdr->ttl, rtt);
	return 0;
}


int receive_ping(int sockfd, int seq) {
	char                buffer[1024];
	struct sockaddr_in  addr;
	socklen_t           addr_len;
	struct timeval      tv_recv;
	struct icmphdr      *icmp_hdr;
	struct iphdr        *ip_hdr;
	int                 ret;
	int                 ip_header_len;

	// ICMP Handling
	while (1) {
		addr_len = sizeof(addr);
		ret = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
		gettimeofday(&tv_recv, NULL);
		if (ret < 0) {
			//recvfrom return
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("Request timeout for icmp_seq %d\n", seq);
				return -1;
			}
			printf("%s","recvfrom");
			return -1;
		}
		//handle header + packet
		ip_hdr = (struct iphdr *)buffer;
		ip_header_len = ip_hdr->ihl * 4;  // IP header length in bytes
		icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);
		
		// Handle ICMP Echo Reply
		if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == (getpid() & 0xFFFF) && icmp_hdr->un.echo.sequence == seq) {
			process_valid_icmp_echo_reply(buffer, ip_header_len, ret, seq, tv_recv, ip_hdr);
			return 0;
		}
		
		// Handle ICMP Error Messages (verbose mode)
		if (g_stats.verbose) {
			if (verbose_handler(buffer, ip_header_len, icmp_hdr, &addr, seq) == -1) {
			return -1;
			}
		}
	}
}
