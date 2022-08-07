#include "ping.h"

struct s_stats stats;


#define __null 0


void handle_sigint(int sig)
{  
	(void)sig;
	struct timeval end;
	gettimeofday(&end,NULL);
	unsigned int duration_secondes  =   +end.tv_sec - stats.start.tv_sec  ;
	int duration_usec =   ((end.tv_sec - stats.start.tv_sec ) /1000) > 100    ? stats.start.tv_usec -  end.tv_usec   : 
  		end.tv_usec - stats.start.tv_usec  ; 
	printf("time  %u.%ums\n" , duration_secondes, duration_usec / 1000);
	free (stats.ip);
	exit(EXIT_SUCCESS);
}




void handle_sigkill(int sig)
{

	   #define MAXSTACKSIZE (16)
    void *stackTraces[MAXSTACKSIZE];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(stackTraces, MAXSTACKSIZE);
}


/*
 * @struct timeval get_time_diff(struct timeval start, struct timeval end)
 * @brief calcul de la difference de temps entre deux temps
 * @param start : temps de depart
 * @param end : temps d'arrivee
 * @return struct timeval : difference de temps
* */

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




#include <sys/uio.h>
#include <sys/socket.h>


/*
 * Checksum routine for ICMP packets.
 * */

void init_stats(int argc , char **argv)
{
	(void)argc,
	(void)argv;
	stats.ip = hostname_to_ipv6(argv[1]);
	stats.success 				= 0,
	stats.total_packets 		= 0,
	stats.failed 				= 0,
	gettimeofday(&stats.start, NULL);
}

void  print_ligne_intermediaire(void)
{
	suseconds_t diff =  (stats.timediff.recieved.tv_usec-  stats.timediff.sent.tv_usec)  /10;
	stats.total_packets += 1;
	size_t len = sizeof(	struct icmphdr);
	printf("\n [%zu ]icmp.seq=%u time=%lu.%lu ms\n" ,len , stats.total_packets  ,diff / 100 ,   diff % 100);
}

























int main(int argc , char **argv)
{
	signal(SIGINT, handle_sigint);
	signal(SIGTTOU, handle_sigkill);
	init_stats(argc,  argv);
	printf( "IP = [%s]" , stats.ip );
	if(stats.ip)
	while(1)
	{
		pinger(stats.ip);
		print_ligne_intermediaire();
		sleep(1);
	}
	return(0);
}
