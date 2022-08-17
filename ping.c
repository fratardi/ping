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




// void handle_sigkill(int sig)
// {

// 	   #define MAXSTACKSIZE (16)
//     void *stackTraces[MAXSTACKSIZE];
//     size_t size;

//     // get void*'s for all entries on the stack
//     size = backtrace(stackTraces, MAXSTACKSIZE);
// }


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



// print readable date from time_t
void print_date(time_t t)
{
	struct tm *tmp = localtime(&t);
	printf("%d-%02d-%02d %02d:%02d:%02d\n", tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}


// unsigned int count_block


/// print content of a stack structure
void print_stat_content(struct stat *stat)
{
	printf("st_dev %lu\n", stat->st_dev);
	printf("st_ino %lu\n", stat->st_ino);
	printf("st_mode %u\n", stat->st_mode);
	printf("st_nlink %lu\n", stat->st_nlink);
	printf("st_uid %u\n", stat->st_uid);
	printf("st_gid %u\n", stat->st_gid);
	printf("st_rdev %lu\n", stat->st_rdev);
	printf("st_size %lu\n", stat->st_size);
	printf("st_blksize %lu\n", stat->st_blksize);
	printf("st_blocks %lu\n", stat->st_blocks);
	printf("st_atime %lu  ", stat->st_atime);
		print_date(stat->st_atime);

	printf("st_mtime %lu  ", stat->st_mtime);
		print_date(stat->st_mtime);
	printf("st_ctime %lu  ", stat->st_ctime);

	print_date(stat->st_mtime);


};


/// get chmod of file
int get_chmod(char *file)
{
	struct stat buf;
	if (stat(file, &buf) == -1)
		return -1;

	print_stat_content(&buf);

	return buf.st_mode;
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







/// read the content o a file and return it in a buffer
char *read_file(char *file)
{
	struct stat buf;
	if (stat(file, &buf) == -1)
		return NULL;
	char *buffer = malloc(buf.st_size);
	if (buffer == NULL)
		return NULL;
	int fd = open(file, O_RDONLY);
	if (fd == -1)
		return NULL;
	read(fd, buffer, buf.st_size);

	write(1, buffer, buf.st_size);

	close(fd);
	return buffer;
}

















int main(int argc , char **argv)
{


read_file(argv[1]);

get_chmod(argv[1]);


	signal(SIGINT, handle_sigint);
//	signal(SIGKILL, handle_sigkill);
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
