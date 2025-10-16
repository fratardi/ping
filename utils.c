#include "ping.h"

unsigned short calculate_checksum(void *packet, int len) {
	unsigned int    sum ;
	unsigned short  *buf;
	unsigned short  ret;
	
	buf = packet;
	sum = 0;
	while (len > 1) {
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	ret = ~sum;
	return ret;
}

char *resolve_hostname(const char *hostname) {
	struct  addrinfo         flags;
	struct  addrinfo         *result;
	struct  addrinfo         *rp;
	char                     *ip_str;
	int                      ret;

	memset(&flags, 0, sizeof(flags));
	flags.ai_family     = AF_INET;
	flags.ai_socktype   = SOCK_DGRAM;

	ret = getaddrinfo(hostname, NULL, &flags, &result);
	if (ret != 0) {
		return NULL;
	}

	rp = result;
	while (rp != NULL) {
		if (rp->ai_family == AF_INET) {
			struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
			ip_str = strdup(inet_ntoa(addr->sin_addr));
			freeaddrinfo(result);
			return ip_str;
		}
		rp = rp->ai_next;
	}

	freeaddrinfo(result);
	return NULL;
}


int verbose_handler(char *buffer, int ip_header_len, struct icmphdr *icmp_hdr, struct sockaddr_in *addr, int seq)
{
   if (g_stats.verbose) {
			struct iphdr *inner_ip;
			struct icmphdr *inner_icmp;
			
			if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
				// Extract the inner IP header from the ICMP payload
				inner_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
				inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
				
				// Verify this error is for our packet
				if (inner_icmp->un.echo.id == (getpid() & 0xFFFF) && inner_icmp->un.echo.sequence == seq) {
					if (icmp_hdr->code == ICMP_EXC_TTL) {
						printf("From %s icmp_seq=%d Time to live exceeded\n", 
							   inet_ntoa(addr->sin_addr), seq);
					} else {
						printf("From %s icmp_seq=%d Time exceeded (code %d)\n", 
							   inet_ntoa(addr->sin_addr), seq, icmp_hdr->code);
					}
					return -1;
				}
			} else if (icmp_hdr->type == ICMP_DEST_UNREACH) {
				inner_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
				inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
				
				if (inner_icmp->un.echo.id == (getpid() & 0xFFFF) && inner_icmp->un.echo.sequence == seq) {
					switch (icmp_hdr->code) {
						case ICMP_NET_UNREACH:
							printf("From %s icmp_seq=%d Destination Net Unreachable\n", 
								   inet_ntoa(addr->sin_addr), seq);
							break;
						case ICMP_HOST_UNREACH:
							printf("From %s icmp_seq=%d Destination Host Unreachable\n", 
								   inet_ntoa(addr->sin_addr), seq);
							break;
						case ICMP_PROT_UNREACH:
							printf("From %s icmp_seq=%d Destination Protocol Unreachable\n", 
								   inet_ntoa(addr->sin_addr), seq);
							break;
						case ICMP_PORT_UNREACH:
							printf("From %s icmp_seq=%d Destination Port Unreachable\n", 
								   inet_ntoa(addr->sin_addr), seq);
							break;
						default:
							printf("From %s icmp_seq=%d Destination Unreachable (code %d)\n", 
								   inet_ntoa(addr->sin_addr), seq, icmp_hdr->code);
							break;
					}
					return -1;
				}
			} else if (icmp_hdr->type == ICMP_SOURCE_QUENCH) {
				inner_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
				inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
				
				if (inner_icmp->un.echo.id == (getpid() & 0xFFFF) && inner_icmp->un.echo.sequence == seq) {
					printf("From %s icmp_seq=%d Source Quench\n", 
						   inet_ntoa(addr->sin_addr), seq);
					return -1;
				}
			} else if (icmp_hdr->type == ICMP_REDIRECT) {
				inner_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
				inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
				
				if (inner_icmp->un.echo.id == (getpid() & 0xFFFF) && inner_icmp->un.echo.sequence == seq) {
					printf("From %s icmp_seq=%d Redirect (code %d)\n", 
						   inet_ntoa(addr->sin_addr), seq, icmp_hdr->code);
					return -1;
				}
			} else if (icmp_hdr->type == ICMP_PARAMETERPROB) {
				inner_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
				inner_icmp = (struct icmphdr *)((char *)inner_ip + (inner_ip->ihl * 4));
				
				if (inner_icmp->un.echo.id == (getpid() & 0xFFFF) && inner_icmp->un.echo.sequence == seq) {
					printf("From %s icmp_seq=%d Parameter Problem\n", 
						   inet_ntoa(addr->sin_addr), seq);
					return -1;
				}
			}
		}
	return 0;
}


void print_stats(void) {
	double avg_rtt;
	double packet_loss;
	double dev;
	double deviation;
	packet_loss = 0;
	printf("\n--- %s ping statistics ---\n", g_stats.hostname);
	if (g_stats.packets_sent > 0) {
		packet_loss = ((double)(g_stats.packets_sent - g_stats.packets_received) / (double)g_stats.packets_sent) * 100.0;
	}
	printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n",
		   g_stats.packets_sent, g_stats.packets_received, packet_loss);
	if (g_stats.packets_received > 0) {
		avg_rtt = g_stats.sum_rtt / g_stats.packets_received;
		deviation = (g_stats.sum_sq_rtt / g_stats.packets_received) - (avg_rtt * avg_rtt);
		dev = sqrt(deviation > 0 ? deviation : 0);
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			   g_stats.min_rtt, avg_rtt, g_stats.max_rtt, dev);
	}
}

void print_usage(void) {
	printf( "Usage: ./ft_ping [-v] [-?] [-c count] [-i interval] [-ttl N] destination\n");
	printf( "Options:\n");
	printf( "  -v            Verbose output (show packet errors)\n");
	printf( "  -?            Display this help message\n");
	printf( "  -c count      Stop after sending count packets\n");
	printf( "  -i interval   Wait interval seconds between sending each packet (default: 1.0)\n");
	printf( "  -ttl N        Set Time To Live (1-255)\n");
	exit(1);
}
