#ifndef __SOCKET_H__
#define __SOCKET_H__

class Socket
{
 public:
   Socket();
   Socket(char *, int);
   ~Socket();
   int connect();
   int disconnect();
   int readPacket();
   int writePacket();
 public:
   char *ip;
   int port;
};

#endif
