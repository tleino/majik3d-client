#include <stdio.h>
#include <stdlib.h>
#include "Socket.hpp"
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

int Socket::connect()
{
   return 1;
}

int Socket::disconnect()
{
   return 1;
}

int readPacket()
{
   return 1;
}

int writePacket()
{
   return 1;
}
