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
/*
 * @struct timeval get_time_diff(struct timeval start, struct timeval end)
 * @brief calcul de la difference de temps entre deux temps
 * @param start : temps de depart
 * @param end : temps d'arrivee
 * @return struct timeval : difference de temps
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


//
