#ifndef SOCK_H
#define SOCK_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>


class plug {

  public:
    int fd;
    int link(const char* host, int port); 	/* Connect */
    int unlink(); 				/* Disconnect */
    int in(char* buf); 				/* Read line (plus added nul char) */
    int out(const char* buf); 			/* Write line */
    plug& operator <<(const char* buf);
    plug();

  private:
    int bi;
    char buffer[512];
};


#endif
