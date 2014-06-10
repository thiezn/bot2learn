/*  

This library contains functions for easy handling of raw sockets. Its almost fully taken from the Covert Client script at http://sock-raw.org/projects/cc/cc.c.html so all credit goes out to them! raelly like the way they've extensively commented the files and seem to do some proper programming that I'll never be able to come up with by myself! :)

*/

#include "rawsock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>




void rawsock::clean_exit(const char *msg, int err)
{
        if (err == 1)
                perror(msg);
        else
                fprintf(stderr, "%s \n", msg);

        exit(EXIT_FAILURE);
}


uint16_t rawsock::checksum_comp(uint16_t *addr, int len) {   /*  compute TCP header checksum */
                                                        /*  with the usual algorithm a bit changed */
                                                        /*  for byte ordering problem resolving */
                                                        /*  see RFC 1071 for more info */
      /* Compute Internet Checksum for "count" bytes
       *         beginning at location "addr".
       */
	register long sum = 0;
	int count = len;
	uint16_t temp;

	while (count > 1)  {
		temp = htons(*addr++);   // in this line:added -> htons
		sum += temp;
		count -= 2;
	}

        /*  Add left-over byte, if any */
	if (count > 0)
		sum += *(unsigned char *) addr;

	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	uint16_t checksum = ~sum;
	return checksum;
}




void rawsock::sendSYN(const char *srcaddr, const char *dstaddr, unsigned short int port)
{

	/* this code experiments with making raw structs by using 
	 * the struct raw_pack (a combination of tcphdr and iphdr) and 
	 * using pointers which is faster compared to using plain structs
	 * and moving them around - this technique is also 
	 * used in the syn scanner Creeper (by ithilgore)
	 * the only new idea is using the combination of the 2 headers
	 * into a struct called raw_pack which makes the code more readable
	 */

	/* The logic is simple: we create a buffer datagram into which the 
	 * tcp and ip headers will be stored. We take into account the additional
	 * storage needed by the pseudo header used for the tcp checksumming process.
	 * The raw_pack pointer points to the beginning of the datagram 
	 * and the pseudo_header will point to the end of the raw_pack (still inside
	 * the buffer datagram). 
	 * The sendto() function will send only the portion of the datagram which
	 * contains the raw_pack (tcp and ip headers ) - the pseudo_header never reaches 
	 * the wire
	 */

	char datagram[sizeof(raw_pack) + sizeof(pseudo_hdr)];	/* buffer for the headers */
	raw_pack *raw = (struct raw_pack *) datagram; 		/* point the raw_pack to the datagram */ 
	pseudo_hdr *phdr; 	/* pseudo header pointer */
	
	unsigned int dst, src;	/* integers used for filling in the addresses with inet_pton() */
	struct sockaddr_in sin; /* struct used for the raw socket info */

	memset(datagram, 0, sizeof(datagram)); /* bzero the datagram */
	
	/* convert strings to network ints */ 
	if (inet_pton(AF_INET, srcaddr, (unsigned int *) &dst) < 0) 
		clean_exit("invalid source addr", 0);
	if (inet_pton(AF_INET,  dstaddr, (unsigned int *) &src) < 0)
		clean_exit("invalid dest addr", 0);

	/* main raw packet building */

		/* raw packet creation */
		/* ip header construction */
		/*
		   0                   1                   2                   3   
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |Version|  IHL  |Type of Service|          Total Length         |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |         Identification        |Flags|      Fragment Offset    |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |  Time to Live |    Protocol   |         Header Checksum       |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                       Source Address                          |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                    Destination Address                        |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                    Options                    |    Padding    |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   */

		/* all the values that are over 1 octet need to be network byte ordered */
		raw->ip.ihl = 5;
		raw->ip.version = 4;
		raw->ip.tos = 0;
		raw->ip.tot_len = htons(40);		/* 16 byte value */
		raw->ip.frag_off = 0;			/* no fragment */
		raw->ip.ttl = 64;			/* default value */
		raw->ip.protocol = IPPROTO_TCP;		/* protocol at L4 */
		raw->ip.check = 0;			/* ??not needed in iphdr */
		raw->ip.saddr = (src);		
		raw->ip.daddr = (dst);
		
		/* There was a confusion with using the htonl function on the 
		 * ip addresses: if the addresses are already converted to network-
		 * byte-order (which they are because of the inet_pton() called before)
		 * then calling htonl on them will bring the opposite results, which 
		 * means that the address will be converted to host byte order causing
		 * havoc. In addition if htonl() is called twice on the same network- 
		 * byte-order address the addr won't be converted back to a network byte addr
		 * as seemingly expected ( following a 2 negatives make 1 positive logic ).
		 * Now i am beginning to understand Hobbit's ranting about the bsd sockets
		 * api .....
			raw->ip.saddr = htonl(src);
			raw->ip.daddr = htonl(dst);
			^ IT WONT WORK if already in network byte order 
		*/


		/* tcp header construction */
		/*
		   0                   1                   2                   3   
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |          Source Port          |       Destination Port        |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                        Sequence Number                        |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                    Acknowledgment Number                      |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |  Data |           |U|A|P|R|S|F|                               |
		   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
		   |       |           |G|K|H|T|N|N|                               |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |           Checksum            |         Urgent Pointer        |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                    Options                    |    Padding    |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   |                             data                              |
		   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		   */

		raw->tcp.source = htons( 1 + (int) (10000.0 * rand() / (RAND_MAX + 1.0)) );
		raw->tcp.dest = htons(port);
		raw->tcp.seq = random();		/* we generate a random sequence number */
		raw->tcp.ack_seq = 0;
		raw->tcp.res1 = 0;			/* reserved bits */
		raw->tcp.doff = 5;			/* header length (counted in 32 bit words). Should be 5 unless TCP options are used */
		raw->tcp.fin = 0;
		raw->tcp.syn = 1;			/* we're going to send a initial three-way-handshake SYN packet */
		raw->tcp.rst = 0;
		raw->tcp.psh = 0;
		raw->tcp.ack = 0;
		raw->tcp.urg = 0;
		raw->tcp.window = htons(512);		/* window size of 512 */
		raw->tcp.check = 0;			/* tcp checksum should be set to 0 initially. It will be calculated further on */
		raw->tcp.urg_ptr = 0;

		/* fill the socket struct */
		sin.sin_family = AF_INET;
		sin.sin_port = raw->tcp.source;
		sin.sin_addr.s_addr = raw->ip.daddr;   

		/* ip header checksum */
		raw->ip.check = htons (checksum_comp((unsigned short *) &(raw->ip), 20));
		// TODO: some hosts respond with incorrect checksum ??? 

		/* pseudo header used for TCP header checksum.  */
		phdr = (struct pseudo_hdr *) (datagram + sizeof(raw_pack));

		phdr->src = raw->ip.saddr;
		phdr->dst = raw->ip.daddr;
		phdr->mbz = 0;
		phdr->proto = IPPROTO_TCP;
		phdr->len = ntohs(0x14);		// TODO: htons(sizeof(struct tcphdr)+size);

		/* tcp checksum */			// TODO: Don't think this will work when you have added a payload
		raw->tcp.check = htons	(checksum_comp((unsigned short *) &(raw->tcp), sizeof(raw->tcp) + sizeof(pseudo_hdr)));

		/* send the raw packet */

		int err = sendto(sockfd, datagram, sizeof(raw_pack), 0, (struct sockaddr *)&sin, sizeof(sin));
		if (err < 0)
			clean_exit("sendto error: ", 1);

}

rawsock::rawsock()
{

  /* make a raw socket */
  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
  {
    clean_exit("can't establish socket", 0);
  }

}

rawsock::~rawsock()
{
  close(sockfd);
}
