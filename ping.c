
#include "ping.h"

t_ping_stats g_stats;
volatile sig_atomic_t g_running = 1;

void signal_handler(int sig) {
	(void)sig;
	g_running = 0;
}


int main(int argc, char **argv) {
	int                 sockfd;
	struct sockaddr_in  addr;
	int                 count ;
	
	count = 0;
	init_g_stats(argc,  argv);
	parse_args(argc, argv);
	
	if (geteuid() != 0) {
		printf( "Need euid == 0\n");
		return -1;
	}
	g_stats.ip_addr = resolve_hostname(g_stats.hostname);
	if (g_stats.ip_addr == NULL) {
		printf( "ft_ping: unknown host %s\n", g_stats.hostname);
		return -1;
	}
	sockfd = create_icmp_socket();
	if (sockfd < 0) {
		return -1;
	}
	// Set TTL
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &g_stats.ttl, sizeof(g_stats.ttl)) != 0) {
		printf("%s","setsockopt");
		close(sockfd);
		return -1;
	}
	// Setup destination address
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(g_stats.ip_addr);
	signal(SIGINT, signal_handler);
	printf("PING %s (%s): %d data bytes\n", g_stats.hostname, g_stats.ip_addr, PACKET_SIZE - 8);
	gettimeofday(&g_stats.start_time, NULL);
	while (g_running && (g_stats.count == 0 || count < g_stats.count)) {
		send_ping(sockfd, &addr, count);
		if (receive_ping(sockfd, count) == 0) {
			g_stats.packets_received++;
		}
		count++;

		if (g_stats.count == 0 || count < g_stats.count) // if packets left to send sleep]
		{
			usleep((unsigned int)(g_stats.interval * 1000000));
		}
	}
	close(sockfd);
	print_stats();
	return 0;
}
