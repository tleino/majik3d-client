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
	 debug->put("Socket constructor");
   #endif
}

Socket::Socket(char *addr, int nport)
{
   ip = new char[strlen(addr)];
   strcpy(ip, addr);
   port = nport;
   #ifdef DEBUG
	 debug->put("Socket constructor: ip=%s, port=%d", ip, port);
   #endif
}

Socket::~Socket()
{
   if(ip != NULL)
	 {
		delete [] ip;
		ip = NULL;
	 }
   #ifdef DEBUG
	 debug->put("Socket destructor");
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
