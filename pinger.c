
#include "ping.h"

extern struct s_stats stats;




/// 

// IPPROTO_IP
//     Internet protocol.
// IPPROTO_IPV6
//     [IP6] [Option Start] Internet Protocol Version 6. [Option End]
int set_ttl(int sock, int n)
{
	int ret;
	ret = setsockopt(sock, IPPROTO_ICMP	, IP_TTL, &n, sizeof(n));
	if (ret == -1)
		perror("setsockopt");
	return ret;
}

// int get_sock_ttl(int socket)
// {

	
// 		int ret;
// 	ret = getsockopt(socket, IPPROTO_ICMP	,  6, 0, 0);
// 	if (ret == -1)
// 		perror("getsockopt");
// 	return ret;

// }



// print sockaddr_in struct	
void print_sockaddr_in(struct sockaddr_in *addr)
{
	printf("sin_family: %d\n", addr->sin_family);
	printf("sin_port: %d\n", addr->sin_port);
	printf("sin_addr: %s\n", inet_ntoa(addr->sin_addr));
}

// print in6_addr struct
void print_in6_addr(struct in6_addr *addr)
{
	int i;
	for (i = 0; i < 16; i++)
		printf("%02x", addr->s6_addr[i]);
	printf("\n");
}

// print sockaddr_in6 struct
void print_sockaddr_in6(struct sockaddr_in6 *addr)
{

	printf("sin6_port: %d\n", addr->sin6_port);
	printf("sin6_flowinfo: %d\n", addr->sin6_flowinfo);
//	print_in6_addr(&addr->sin6_addr);
	printf("sin6_scope_id: %d\n", addr->sin6_scope_id);
}


// returns the checksum of a icmp header
unsigned short checksum(unsigned short *buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}

// print icmp struct
void print_icmp(struct icmp *icmp)
{


	ICMP_INFOTYPE(icmp->icmp_type);

	printf("\n=====================\nREPLY\nICMP_INFOTYPE%d " , 	ICMP_INFOTYPE(icmp->icmp_type));
	printf("icmp_type: %hhd   \n", icmp->icmp_type   );
	printf("icmp_code: %hu\n", icmp->icmp_code);






	printf("REALSUM %hu\n", checksum((unsigned short *)icmp,39 + 8));
	printf("icmp_cksum: %hu\n", icmp->icmp_cksum);

	printf("icmp->icmp_pptr: %d\n", icmp->icmp_pptr);
	printf("icmp->icmp_hun.ih_gwaddr.s_addr: %d\n"  , icmp->icmp_hun.ih_gwaddr.s_addr);

	printf("icmp->icmp_id: %d\n"  , icmp->icmp_id);
	printf("icmp->icmp_seq: %d\n"  , icmp->icmp_seq);

	printf("icmp->icmp_void : %d\n"  , icmp->icmp_void);

	printf("icmp->icmp_pm_void: %d\n"  , icmp->icmp_pmvoid);
	printf("icmp->icmp_nextmtu: %d\n"  ,icmp->icmp_nextmtu);

	printf("icmp->icmp_num_addrs: %d\n"  , icmp->icmp_num_addrs);
	printf("icmp->icmp_wpa: %d\n"  , icmp->icmp_wpa);
	printf("icmp->icmp_lifetime: %d\n"  , icmp->icmp_lifetime);



	printf("icmp->icmp__otime: %u\n"  , icmp->icmp_otime);
	printf("icmp->icmp_rtime %u\n"  , icmp->icmp_rtime);
	printf("icmp->icmp_ttime: %u\n"  , icmp->icmp_ttime);

 printf("icmp->_data %s\n"  , icmp->icmp_data);

printf("icmp->icmp_dun.id_data[0] %d\n" , icmp->icmp_dun.id_data[0]);



//icmp->icmp_hun.ih_gwaddr.s_addr;
	printf("icmp_id: %d\n", icmp->icmp_id);
	printf("icmp_seq: %d\n", icmp->icmp_seq);
}



//hostname to ipv4 address
int hostname_to_ipv4(char *hostname, struct in_addr *addr)
{
	struct hostent *h;
	h = gethostbyname(hostname);
	if (h == NULL)
		return -1;
	addr->s_addr = *(long *)h->h_addr;
	return 0;
}

//hostname to ipv6 address
int hostname_to_ip6(char *hostname, struct in6_addr *addr)
{
	struct hostent *h;
	h = gethostbyname2(hostname, AF_INET6);
	if (h == NULL)
		return -1;
	memcpy(addr, h->h_addr, h->h_length);
	return 0;
}





// get socketinfo
void get_socketinfo(int sock)
{

	printf("get_socket_info\n");



	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int ret;
	ret = getsockname(sock, (struct sockaddr *)&addr, &addrlen);
	if (ret == -1)
		perror("getsockname");
	else
		print_sockaddr_in(&addr);
}




int open_icmp_socket()
{
	int sock;
	sock = socket(AF_INET,  SOCK_DGRAM, IPPROTO_ICMP);
	if (sock == -1)
		perror("socket");

	get_socketinfo(sock);


	return sock;
}



// function that listen on given socket for ICMP packet
int listen_icmp(int sock)
{
	int ret;
	char buf[BUFFER_SIZE];
	struct sockaddr_in6 addr;
	socklen_t addr_len = sizeof(addr);
	ret = recvfrom(sock, buf, BUFFER_SIZE,MSG_WAITFORONE, (struct sockaddr *)&addr, &addr_len);
//		ret = recvfrom(sock, buf, BUFFER_SIZE,MSG_WAITFORONE, (struct sockaddr *)&addr, &addr_len);
// MSG_WAITFORONE
//MSG_WAITALL
	if (ret == -1)
		perror("recvfrom");
	else
	{
		printf("officially received %d bytes\n", ret);
		//print_sockaddr_in6(&addr);
	//	print_icmp((struct icmp *)buf);

	}

	if (ret == -1)
		perror("recvfrom");
	return ret;
}


// get the curent ttl info of a ipv4 socket
int get_ttl(int sock)
{
	int ret;
	int ttl = -1;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	ret = getsockname(sock, (struct sockaddr *)&addr, &addrlen);
	if (ret == -1)
		perror("getsockname");
	else
		ttl = addr.sin_addr.s_addr;



	return ttl;
}



int pinger(char *str )
{


	int 	sock = 0 ;
	int		alen = 0;

	unsigned char *packet_buffer = NULL;
	char addrbuf    [INET6_ADDRSTRLEN] = "hellllllllllomy name is rebecca\0"; //message you want to send in ? 
	char hostname[NI_MAXHOST];
	struct icmphdr *icp = NULL;
	struct addrinfo* result = NULL;
	struct addrinfo* res;
	struct sockaddr_in source ;
	struct sockaddr_in dst  ;
	size_t	datalen = strlen(addrbuf);
	int 	packlen = datalen + sizeof(struct icmphdr);
	source 	.sin_family = AF_INET ;
	dst		.sin_family = AF_INET;
	dst		.sin_port 	= htons(NI_MAXHOST);



/// fits all the data about the host in the result struct
	getaddrinfo(str, NULL, NULL, &result);	



	res = result;


	printf("hostname: %s\n res->ai_addr\n", hostname);

	if (inet_aton(str, &dst.sin_addr) == 0) {
		fprintf(stderr, "The first argument must be an IP address\n");
		exit(1);
	}
	sock = open_icmp_socket();
	printf(" socket == %d \n", sock);

	if (sock == -1) {
		perror("Error creating socket");
	}


	
	if (getsockname(sock, (struct sockaddr*)&source, (unsigned int *)&alen) == -1) 
	{
			perror("getsockname");
			exit(2);
	}

		if (!(packet_buffer = (unsigned char *)malloc((unsigned int)packlen))) {
		fprintf(stderr, "ping: out of memory.\n");
		exit(2);
	}

    icp = (struct icmphdr *)packet_buffer;

	init_icp_header(icp);


	gettimeofday(&stats.timediff.sent,NULL);
	int cc = datalen + 8;
	//set_ttl(sock ,29);


// /int ttl = 12; /* max = 255 */

 struct timeval tv_out;
    tv_out.tv_sec = 0;//RECV_TIMEOUT;
    tv_out.tv_usec = 0;

 printf("setsockopt [%d]\n ",   setsockopt(sock, SOL_IP, SO_RCVTIMEO,
                   (const char*)&tv_out, sizeof tv_out));

//printf("setsockopt [%d]\n " , 
   int ttl = 15;		     /*	max = 255 */
     setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
//);




	//printf( "TTL  = %d "   , get_sock_ttl(sock));


	size_t  mch  =   sendto(sock, icp, cc, 8, (struct sockaddr*)&dst, sizeof(dst));
	if(!mch)
	{
		perror("sendto");
		exit(2);
	}

	printf(" TTL = %d" , get_ttl(sock));
	
	
	
	/// wait message from the server
	listen_icmp(sock) ;



	printf("sendtoret == %zu  legnth sent cc  == %d \n", mch, cc);

	printf("addrbuff %s" , addrbuf );


	gettimeofday(&stats.timediff.recieved,NULL);

	close(sock);
	print_ligne_intermediaire();
		return(1);
}