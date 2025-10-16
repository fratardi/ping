#include "ping.h"

char *getopt1(char *str, char *opt)
{
	char *p = str;
	char *q = opt;
	
	while (*p && *q)
		if (*p++ != *q++)
			return NULL;
	if (!*q)
		return p;
	return NULL;
}

unsigned char only_one_valid_place(int argc, char** argv)
{
	unsigned int valid_place_to_ping = 0;
	int i = 1;
	struct in_addr addr;
	
	while (i < argc) {
		if (inet_pton(AF_INET, argv[1], &addr) || inet_pton(AF_INET6, argv[1], &addr)) {
			valid_place_to_ping++;
		}
		i++;
	}
	
	printf("%d\n", valid_place_to_ping);
	return (valid_place_to_ping == 1 ? 0 : 1);
}
