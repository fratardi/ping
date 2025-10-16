#include "ping.h"

char *resolve_hostname(const char *hostname) {
    struct addrinfo         hints;
     struct addrinfo         *result;
     struct addrinfo         *rp;
    char *ip_str;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(hostname, NULL, &hints, &result);
    if (ret != 0) {
        return NULL;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
            ip_str = strdup(inet_ntoa(addr->sin_addr));
            freeaddrinfo(result);
            return ip_str;
        }
    }

    freeaddrinfo(result);
    return NULL;
}

int create_icmp_socket(void) {
    int sockfd;
    struct timeval tv_out;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // Set receive timeout
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void send_ping(int sockfd, struct sockaddr_in *addr, int seq) {
    t_ping_pkt packet;
    struct timeval tv;
    int ret;

    memset(&packet, 0, sizeof(packet));

    // Setup ICMP header
    packet.hdr.type = ICMP_ECHO;
    packet.hdr.code = 0;
    packet.hdr.un.echo.id = getpid() & 0xFFFF;  // Use only lower 16 bits
    packet.hdr.un.echo.sequence = seq;
    packet.hdr.checksum = 0;

    // Add timestamp to payload
    gettimeofday(&tv, NULL);
    memcpy(packet.msg, &tv, sizeof(tv));

    // Fill rest of payload with pattern
    for (size_t i = sizeof(tv); i < sizeof(packet.msg); i++) {
        packet.msg[i] = i;
    }

    // Calculate checksum
    packet.hdr.checksum = calculate_checksum(&packet, sizeof(packet));

    // Send packet
    ret = sendto(sockfd, &packet, sizeof(packet), 0,
                 (struct sockaddr *)addr, sizeof(*addr));
    if (ret < 0) {
        perror("sendto");
    } else {
        g_stats.packets_sent++;
    }
}

int receive_ping(int sockfd, int seq) {
    char buffer[1024];
    struct sockaddr_in addr;
    socklen_t addr_len;
    struct timeval tv_recv, tv_sent;
    struct icmphdr *icmp_hdr;
    struct iphdr *ip_hdr;
    double rtt;
    int ret;
    int ip_header_len;

    // Loop to handle multiple ICMP packets until we find ours
    while (1) {
        addr_len = sizeof(addr);
        ret = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                       (struct sockaddr *)&addr, &addr_len);
        
        gettimeofday(&tv_recv, NULL);

        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Request timeout for icmp_seq %d\n", seq);
                return -1;
            }
            perror("recvfrom");
            return -1;
        }

        // With SOCK_RAW, we receive IP header + ICMP packet
        ip_hdr = (struct iphdr *)buffer;
        ip_header_len = ip_hdr->ihl * 4;  // IP header length in bytes
        icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);

        // Handle ICMP Time Exceeded (TTL expired)
        if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
            char router_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ip_hdr->saddr), router_ip, INET_ADDRSTRLEN);
            printf("From %s icmp_seq=%d Time to live exceeded\n", router_ip, seq);
            return -1;
        }
        
        // Verify it's an ICMP ECHOREPLY and matches our PID
        if (icmp_hdr->type == ICMP_ECHOREPLY && 
            icmp_hdr->un.echo.id == (getpid() & 0xFFFF) &&
            icmp_hdr->un.echo.sequence == seq) {
            
            // Extract timestamp from payload (after ICMP header)
            memcpy(&tv_sent, buffer + ip_header_len + sizeof(struct icmphdr), sizeof(tv_sent));

            // Calculate RTT in milliseconds
            rtt = (tv_recv.tv_sec - tv_sent.tv_sec) * 1000.0 +
                  (tv_recv.tv_usec - tv_sent.tv_usec) / 1000.0;

            // Update statistics
            if (rtt < g_stats.min_rtt) g_stats.min_rtt = rtt;
            if (rtt > g_stats.max_rtt) g_stats.max_rtt = rtt;
            g_stats.sum_rtt += rtt;
            g_stats.sum_sq_rtt += rtt * rtt;

            printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                   ret - ip_header_len, g_stats.ip_addr, seq, ip_hdr->ttl, rtt);

            return 0;
        }
        // Continue looping to wait for our packet
    }
}
