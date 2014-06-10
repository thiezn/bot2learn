// see rawsock.cpp file for module description

#ifndef RAWSOCK_H
#define RAWSOCK_H

#include <sys/socket.h>
#include <netinet/ip.h> 		// linux ip header struct
#include <netinet/tcp.h>        	// linux tcp header struct

class rawsock
{
  public:

  int sockfd;						// the socket file descriptor

  rawsock();
  virtual ~rawsock();

  void clean_exit(const char *, int);				// exit program with string (e.g. exit on error or normal exit

  void sendSYN(const char *srcaddr, const char *dstaddr, unsigned short int port);

  private:

    /* structure containing both the IP and the TCP header. */
    typedef struct raw_pack {           
      struct iphdr ip;
      struct tcphdr tcp;
    } raw_pack;


    /* pseudo header used for calculating the TCP checksum */
    /* this header never reaches the wire */
    typedef struct pseudo_hdr {
      u_int32_t src;
      u_int32_t dst;
      u_char mbz;
      u_char proto;
      u_int16_t len;
    } pseudo_hdr;


    uint16_t checksum_comp(uint16_t *, int);              // Calculate header checksum


};

#endif

