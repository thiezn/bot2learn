#include "sock.h"


plug::plug() 
{ 
  bi = 0; 
}


int plug::link(const char *host, int port)
{
  struct sockaddr_in si;
  struct hostent *hostess_twinkies = gethostbyname(host);

  fd = socket(PF_INET, SOCK_STREAM, 0);
  si.sin_family = AF_INET;
  si.sin_port = htons(port);
  si.sin_addr = *((struct in_addr *) hostess_twinkies->h_addr);
  memset(&(si.sin_zero),0,8);

  
  if(connect(fd, (struct sockaddr *)&si, sizeof(struct sockaddr)) == -1) 
  {
     fd=0;
  }
  
  return fd;
}

int plug::unlink()
{
  return close(fd);
}

int plug::in(char *buf)
{
  int len;
  
  for(len=0; recv(fd,buf,1,0); len++, buf++)
  {
    if(*buf == '\n')
    {
      if(*(buf-1) == '\r')
      {
        *(buf+1) = 0;
        return len;
      }
    }
  }
}


int plug::out(const char *buf)
{
  int i;
  for(i=0; *(short *)buf != 0x0a0d; i++, buf++)
  {
    buf -= i; 
    i += 2;
  }
  return send(fd, buf, i, 0);
}

plug& plug::operator <<(const char* buf)
{
  int len = strlen(buf);
  memmove(buffer+bi, buf, len);
  bi += len;
  
  if(buf[len - 1] == '\n' && buf[len - 2] == '\r')
  {
    send(fd, buffer, bi, 0);
    bi = 0;
  }

  return *this;
}
