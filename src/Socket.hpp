#ifndef __SOCKET_H__
#define __SOCKET_H__

class Socket
{
 public:
   Socket();
   Socket(char *, int);
   ~Socket();
   void connect();
   void disconnect();
   void readPacket();
   void writePacket();
 public:
   char *ip;
   int port;
};

#endif
