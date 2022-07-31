#include "ping.h"

struct s_stats stats;



void handle_sigint(int sig)
{  
	(void)sig;
	struct timeval end;
	gettimeofday(&end,NULL);
	unsigned int duration_secondes  =   +end.tv_sec - stats.start.tv_sec  ;
	int duration_usec =   ((end.tv_sec - stats.start.tv_sec ) /1000) > 100    ? stats.start.tv_usec -  end.tv_usec   : 
  		end.tv_usec - stats.start.tv_usec  ; 
	printf("\ntime  %u.%ums\n" , duration_secondes, duration_usec / 1000);
	free (stats.ip);
	free (stats. hostname);
	
	free (stats.packet_buffer);
	exit(EXIT_SUCCESS);
}
/*
 * @struct timeval get_time_diff(struct timeval start, struct timeval end)
 * @brief calcul de la difference de temps entre deux temps
 * @param start : temps de depart
 * @param end : temps d'arrivee
 * @return struct timeval : difference de temps
* */


// generates a random int between 1 and 4 
int get_random()
{
	return (rand() % 4) + 1;
}



/*
 * Checksum routine for ICMP packets.
 * */

void init_stats(int argc , char **argv)
{
	(void)argc,
	(void)argv;
	
stats.packlen = 200;
		if (!(stats.packet_buffer = (unsigned char *)malloc((unsigned int)stats.packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}


	stats.ip = hostname_to_ipv6(argv[1]);
	printf(
		"PING %s (%s): %d(%d) bytes of data.\n",
		argv[1],
		stats.ip,
		PING_PKT_S,
		PING_PKT_S + 28
	);

	stats.success 				= 0,
	stats.total_packets 		= 0,
	stats.failed 				= 0,
	gettimeofday(&stats.start, NULL);

}

void init_struct(int argc , char ** argv )
{
(void)	argv; 
(void)	argc;
	stats.arg = argv[1];
	stats.ip = hostname_to_ipv6(argv[1]);
	stats. hostname = ipv4_to_hostname(stats.ip);
	printf("INIT STATS[%s]  [%s] \n\n" , stats.ip, ipv4_to_hostname(stats.ip));


	

}



void  print_ligne_intermediaire(void)
{
	suseconds_t diff =  (stats.timediff.recieved.tv_usec-  stats.timediff.sent.tv_usec)  /10;
	stats.total_packets += 1;
	size_t len = sizeof(	struct icmphdr);
	printf("\n [%zu ]icmp.seq=%u time=%lu.%lu ms\n" ,len , stats.total_packets  ,diff / 100 ,   diff % 100);
}



void * func(void *arg )
{
	int time = get_random();

	printf(" sleeping %d \n", time);
	printf("\nT1hreads are equal.\n");

	pinger(stats.ip);
	
	printf("\nT2hreads are equal\n");
	return(NULL);
}

void send_ping(void)
{

	pthread_t ptid;

	memset(&ptid,0 ,sizeof(pid_t));

		printf( "\npthreadcreate == %d \n " , 	
	 	pthread_create(&ptid, NULL, &func, NULL) );
printf("\n PRTID ==  %lu\n" , ptid);
	//	pthread_exit(NULL);

}


int main(int argc , char **argv)
{

	signal(SIGINT, handle_sigint);
	init_struct(argc , argv );
	printf( "IP = [%s]" , stats.ip );
	if(stats.ip)
	while(1)
	{
	//	pinger(stats.ip);
		send_ping();
		sleep(1);
	}
	return(0);
}


//

