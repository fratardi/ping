#include "ping.h"

extern struct s_stats stats;

void init_ping(char **argv)
{
	struct addrinfo* result;
	struct addrinfo* res;
	int error;
	
	/* resolve the domain name into a list of addresses */
	if ((error = getaddrinfo(argv[1], NULL, NULL, &result))) {
		if (error == EAI_SYSTEM) {
			perror("getaddrinfo");
		} else {
			fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
		}
		exit(EXIT_FAILURE);
	}
	
	res = result;
	while (res != NULL) {
		char hostname[NI_MAXHOST];
		error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
		if (error) {
			fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
			continue;
		}
		if (*hostname)
			printf("hostname: %s\n", hostname);
		res = res->ai_next;
	}
	freeaddrinfo(result);
}
