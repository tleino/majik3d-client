#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include "Majik.hpp"

Socket::Socket()
{
   ip = NULL;
   #ifdef DEBUG
	 Debug("Socket constructor");
   #endif
}

Socket::Socket(char *addr, int nport)
{
   strcpy(ip, addr);
   port = nport;
   #ifdef DEBUG
	 Debug("Socket constructor: ip=%s, port=%d", ip, port);
   #endif
}

Socket::~Socket()
{
   if(ip != NULL)
	 free(ip);
   #ifdef DEBUG
	 Debug("Socket destructor");
   #endif
}

void 
Socket::connect()
{
}

void 
Socket::disconnect()
{
}

void 
Socket::readPacket()
{
}

void 
Socket::writePacket()
{
}
