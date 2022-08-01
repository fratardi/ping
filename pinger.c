
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
	print_in6_addr(&addr->sin6_addr);
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








// change endianness of a string of bytes
void change_endianness(char *str, int len)
{
	int i;
	char tmp;
	for (i = 0; i < len / 2; i++) {
		tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = ~tmp;
	}
}






// print icmp struct
void print_icmp(struct icmp *icmp)
{



//change_endianness(icmp->icmp_data , strlen(icmp->icmp_data));

	ICMP_INFOTYPE(icmp->icmp_type);

	printf("\n=====================\nREPLY\nICMP_INFOTYPE%d " , 	ICMP_INFOTYPE(icmp->icmp_type));
	printf("icmp_type: %hhd   \n", icmp->icmp_type   );
	printf("icmp_code: %d\n", icmp->icmp_code);
//	printf("REALSUM %hu\n", checksum((unsigned short *)icmp,12));
	printf("icmp_cksum: %hx\n", icmp->icmp_cksum);

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
	sock = socket(AF_INET,  SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1)
		perror("socket");

	get_socketinfo(sock);


	return sock;
}

// function that prints the content of an msghdr struct
void print_msghdr(struct msghdr *msg)
{
	printf("msg_name: %p\n", msg->msg_name);
	printf("msg_namelen: %d\n", msg->msg_namelen);
	printf("msg_iov: %p\n", msg->msg_iov);
	printf("msg_iovlen: %d\n", msg->msg_iovlen);
	printf("msg_control: %p\n", msg->msg_control);
	printf("msg_controllen: %d\n", msg->msg_controllen);
	printf("msg_flags: %d\n", msg->msg_flags);
}



// function that listen on given socket for ICMP packet
int listen_icmp(int sock)
{
#define BUFFER_SIZE 1024
	struct msghdr msg;

// memset hdr to 0
	memset(&msg, 0, sizeof(msg));


	int ret;
	char buf[BUFFER_SIZE];
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
		//ret =  recvmsg(   sock , &msg , MSG_WAITFORONE) ;

	ret = recvfrom(sock, buf, BUFFER_SIZE,MSG_WAITFORONE, (struct sockaddr *)&addr, &addr_len);



	printf("message size  %d " ,ret );

		


// MSG_WAITFORONE
//MSG_WAITALL
	if (ret == -1)
		perror("recvfrom");
	else
	{
		printf("officially received %d bytes\n", ret);
		print_sockaddr_in6(&addr);
		print_icmp((struct icmp *)buf);

	}

	if (ret == -1)
		perror("recvfrom");
	return ret;
}

// getsthe ttlinfo of a socket
void get_ttlinfo(int sock)
{
	int ret;
	struct timeval ttl;
	socklen_t ttl_len = sizeof(ttl);
	ret = getsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL , &ttl, &ttl_len);
	if (ret == -1)
		perror("getsockopt");
	else
		printf("ttl: %d\n", ttl.tv_sec);
}



int pinger(char *str )
{


	int 	sock = 0 ;
	int		alen = 0;

	unsigned char *packet_buffer = NULL;
	char addrbuf    [INET6_ADDRSTRLEN] = "helloworld\n\0"; //message you want to send in ? 
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
		bzero(packet_buffer, packlen);


    icp = (struct icmphdr *)packet_buffer;

// 0xf7ff;
// 0x7FFF;
// 0xFF7F;
	init_icp_header(icp);
	icp->checksum = 0xFFF7;
	gettimeofday(&stats.timediff.sent,NULL);
	int cc = datalen + 8;
	//set_ttl(sock ,29);


// /int ttl = 12; /* max = 255 */
  int ttl = 254;
 struct timeval tv_out;
    tv_out.tv_sec = 0;//RECV_TIMEOUT;
    tv_out.tv_usec = 0;

	get_ttlinfo(sock);

 setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	setsockopt(sock, SOL_IP, SO_RCVTIMEO,(const char*)&tv_out, sizeof tv_out);
   setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

//);


	get_ttlinfo(sock);


	size_t  mch  =   sendto(sock, icp, cc, 8, (struct sockaddr*)&dst, sizeof(dst));
	if(!mch)
	{
		perror("sendto");
		exit(2);
	}

	
	
	/// wait message from the server
	listen_icmp(sock) ;



	printf("sendtoret == %zu  legnth sent cc  == %d \n", mch, cc);

	printf("addrbuff %s" , addrbuf );


	gettimeofday(&stats.timediff.recieved,NULL);
	free(packet_buffer);
	close(sock);
	print_ligne_intermediaire();
		return(1);
}