#include <stdio.h>
#include <stdlib.h>
#include "Socket.hpp"
#include "Majik.hpp"

Socket::Socket()
{
   ip = NULL;
   #ifdef DEBUG
	 fprintf(stderr, "Socket constructor\n");
   #endif
}

Socket::Socket(char *addr, int nport)
{
   strcpy(ip, addr);
   port = nport;
   #ifdef DEBUG
	 fprintf(stderr, "Socket constructor: ip=%s, port=%d\n", ip, port);
   #endif
}

Socket::~Socket()
{
   if(ip != NULL)
	 free(ip);
   #ifdef DEBUG
	 fprintf(stderr, "Socket destructor\n");
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
