/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>

#if defined(__svr4__)
// For Solaris...
#include <unistd.h>
#include <stropts.h>
#include <sys/filio.h>

// Not sure what this might break, but its set to this value on Linux
// and on DEC/UNIX boxes...
#define INADDR_NONE INADDR_BROADCAST
#endif

#include "Debug.hpp"
#include "Error.hpp"
#include "Socket.hpp"

Socket::Socket()
{
   ip = NULL;
   szBuffer = NULL;
   DEBUG ("Socket constructor");
}

Socket::Socket(char *addr, int nport)
{
   ip = new char[strlen(addr)];
   strcpy(ip, addr);
   port = nport;
   DEBUG (debug->string("Socket constructor: ip=%s port=%d", ip, port));
}

Socket::~Socket()
{
   if(ip != NULL)
	 {
		delete [] ip;
		ip = NULL;
	 }
   
   if(szBuffer != NULL)
     {
		free(szBuffer);
		szBuffer = NULL;
	 }
   
   DEBUG ("Socket destructor");
}

void 
Socket::connectServer()
{
   struct hostent *pHostEnt;
   struct sockaddr_in ServerAddr;
   
   szBuffer = strdup("");
   
   if(inet_addr(ip) == INADDR_NONE) {
		if((pHostEnt = gethostbyname(ip)) != NULL) {
		   delete [] ip;
		   ip = new char [3*4];
		   sprintf(ip, "%d.%d.%d.%d",
				   (unsigned char)pHostEnt->h_addr_list[0][0],
				   (unsigned char)pHostEnt->h_addr_list[0][1],
				   (unsigned char)pHostEnt->h_addr_list[0][2],
				   (unsigned char)pHostEnt->h_addr_list[0][3]);
		}
   }
   
   memset (&ServerAddr, 0, sizeof (ServerAddr));
   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons (port);
   
   if (inet_aton(ip,&ServerAddr.sin_addr) <= 0)
     error->put(ERROR_FATAL, "%s: Unknown host", ip);
   
   if ((nSocket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
     error->put(ERROR_FATAL, "Could not create a socket: %s", strerror (errno));
   
   if (connect (nSocket, (struct sockaddr *) &ServerAddr, sizeof (ServerAddr)) < 0) {
      printf ("Unable to connect to remote host: %s\n", strerror (errno));
	  printf ("Attempting to reconnect...\n");
	  sleep (10);
	  connectServer();
   }
}

void 
Socket::disconnectServer()
{
   if(szBuffer != NULL)
     {
		free(szBuffer);
		szBuffer = NULL;
	 }
      
   // close (nSocket);
}

char *
Socket::readPacket()
{
   unsigned long nLen = 0;
   
   do {
      ioctl (nSocket, FIONREAD, &nLen);
      if (nLen != 0) {
	 char szData[1024*10];
	 long nReceived;
	 if ((nReceived = recv(nSocket,(char *)&szData,sizeof(szData)-1,0)) == -1)
	   error->put(ERROR_FATAL, "Connection closed: %s", strerror (errno));
	 
	 szData[nReceived] = 0;
	 
	 // Resize the buffer
	 szBuffer = (char *) realloc(szBuffer,strlen(szData)+strlen(szBuffer)+1);
	 // Copy the new data
	 strcat(szBuffer,szData);
      }
   } while (nLen != 0);
   
   // Split the data into lines
   char *szReturn = NULL;
   while ((szReturn = strchr(szBuffer,'\r')) != NULL) {
      char *szRet = NULL;
      // Terminate the string
      *(szReturn++) = '\0';
      // Dispatch a new string
      szRet = strdup(szBuffer);
      // Remove new lines
      if (*szReturn == '\n')
		szReturn++;
      memmove(szBuffer,szReturn,strlen(szReturn)+1);
      
      return szRet;
   }
   return NULL;
  
}

void 
Socket::writePacket(char *szFmt,...)
{
   va_list vl;
   char szData[1024*10];
   
   va_start (vl, szFmt);
   vsprintf (szData, szFmt, vl);
   va_end (vl);
   
   sprintf (szData, "%s\r\n", szData);
   
   if (send(nSocket, szData,strlen(szData),0) == -1)
     error->put(ERROR_FATAL, "Could not send data: %s", strerror (errno));
  
}
