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

#include "Socket.hpp"
#include "Debug.hpp"
#include "Error.hpp"
#include "Config.hpp"

#ifdef WIN32	
#include <winsock.h>
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static bool fWsaStarted = false;

static unsigned long __stdcall
NonClassFunctionDummy(void *pNet)
{
  ((Socket *)pNet)->Thread();
  return 0;
}

Socket::Socket(char *szHost,int nPort)
{
  pFirstLine = NULL;
  szBuffer = strdup("");
  this->nPort = nPort;
  this->szHost = strdup(szHost);
  
  if((hWait = CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL)
    {
      error->put (ERROR_FATAL, "CreateEvent failed, low on resources?");
    }
  
  if((LinesSemaphore = CreateSemaphore(NULL,0,10,NULL)) == NULL)
    {
      error->put (ERROR_FATAL, "CreateSemaphore failed, low on resources?");
    }
  
  DWORD nId;
  if((NetThread = CreateThread(NULL,0,&NonClassFunctionDummy,this,0,&nId))
     == NULL)
    {
      error->put (ERROR_FATAL, "CreateThread failed, low on resources?");
    }
  // Wait for the network to initialize.
  WaitForSingleObject(LinesSemaphore,INFINITE);
  
  ReleaseSemaphore(LinesSemaphore,1,NULL);
}

Socket::~Socket()
{
  if(NetThread != NULL)
    {
      TerminateThread(NetThread,0);
    }
  
  free(szHost);
  
  if(LinesSemaphore != NULL)
    {
      CloseHandle(LinesSemaphore);
    }
  
  if(hWait)
    {
      CloseHandle(hWait);
    }
  
  struct LinkedList_s	*pNow = pFirstLine;
  while(pNow != NULL)
    {
      pNow = pNow->pNext;
      free(pFirstLine->szText);
      free(pFirstLine);
      pFirstLine = pNow;
    }
  
  if(szBuffer != NULL)
    {
      free(szBuffer);
      
    }
  
  closesocket(nSocket);
}

char *
Socket::readPacket()
{
  char *szRet;
  WaitForSingleObject(LinesSemaphore,INFINITE);
  
  if(pFirstLine == NULL)
    {
      szRet = NULL;
    }
  else
    {
      struct LinkedList_s *pNow = pFirstLine;
      szRet = pNow->szText;
      pFirstLine = pNow->pNext;
      free(pNow);
      
    }
  
  ReleaseSemaphore(LinesSemaphore,1,NULL);
  
  return szRet;
  
  
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
  
  if(send(nSocket,szData,strlen(szData),0) == SOCKET_ERROR)
    {
      error->put (ERROR_WARNING, "Could not send data over the network.");
    }  
}

void
Socket::Dispatch(char *szLine)
{
  WaitForSingleObject(LinesSemaphore,INFINITE);
  
  struct LinkedList_s	**ppNow = &pFirstLine;
  while(*ppNow != NULL)
    {
      ppNow = &((*ppNow)->pNext);
    }
  *ppNow = (struct LinkedList_s *)malloc(sizeof(struct LinkedList_s));
  (*ppNow)->pNext = NULL;
  (*ppNow)->szText = szLine;
  
  SetEvent(hWait);
  
  ReleaseSemaphore(LinesSemaphore,1,NULL);
  
}

void
Socket::Thread()
{
  struct hostent *pHostEnt;
  unsigned long nAddr;
  struct sockaddr_in ServerAddr;
  
  
  // Startup
  
  if(fWsaStarted == false)
    {
      fWsaStarted = true;
		WSADATA wsaData;
		if(WSAStartup(MAKEWORD( 1, 1 ),&wsaData) != 0)
		{
		  error->put (ERROR_FATAL, "winsock.dll could not be loaded.");
		  return;
		}
    }
  
  // Namelookup
  
  // We dont want to do any unnessery dns requests.
  if(inet_addr(szHost) == INADDR_NONE)
    {
      if((pHostEnt = gethostbyname(szHost)) != NULL)
	{
	  free(szHost);
	  szHost = (char *)malloc(3*4);
	  sprintf(szHost, "%d.%d.%d.%d",
		  (byte)pHostEnt->h_addr_list[0][0],
		  (byte)pHostEnt->h_addr_list[0][1],
		  (byte)pHostEnt->h_addr_list[0][2],
		  (byte)pHostEnt->h_addr_list[0][3]);
	}
    }
  
  if((nAddr = inet_addr(szHost)) == INADDR_NONE)
    {
      error->put (ERROR_FATAL, "Host '%s' not found, can not connect: ",
		  szHost);
    }
  
  // Connect.
  
  if ((nSocket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      error->put (ERROR_FATAL, "Could not create a new socket.");
    }
  
  memset (&ServerAddr, 0, sizeof (ServerAddr));
  ServerAddr.sin_family = AF_INET;
  ServerAddr.sin_port = htons (nPort);
  ServerAddr.sin_addr.S_un.S_addr = nAddr;
  
  if (connect (nSocket, (struct sockaddr *) &ServerAddr, sizeof (ServerAddr))
      < 0)
    {
      error->put (ERROR_FATAL, "Connection to '%s' failed.", szHost);
    }
  
  // Release the other thread.
  ReleaseSemaphore(LinesSemaphore,1,NULL);
  
  // Read.
  
  while(1)
    {
      char szData[1024*10];
      long	nRecived;
      if((nRecived = recv(nSocket,(char *)&szData,sizeof(szData)-1,0))
	 == SOCKET_ERROR)
	{
	  error->put (ERROR_FATAL, "Could not receive data from host, " \
		      "connection closed?");
	}
      szData[nRecived] = 0;
      
      // Resize the buffer
      szBuffer = (char *)realloc(szBuffer,strlen(szData)+strlen(szBuffer)+1);
      // Copy the new data.
      strcat(szBuffer,szData);
      
      // Split the data into lines.
      char *szReturn;
      while((szReturn = strchr(szBuffer,'\r')) != NULL)
	{
	  // Terminate the string.
	  *(szReturn++) = '\0';
	  // Dispatch a new string.
	  Dispatch(strdup(szBuffer));
	  // Im reciving \n's A small cludge to remove them.
	  if(*szReturn == '\n')
	    {
	      szReturn ++;
	    }
	  // Move the old data. 
	  // (I wont bother to resize, that will be done when the next network
	  // packet is recived)
	  memmove(szBuffer,szReturn,strlen(szReturn)+1);
	}
    }
}

#else
/* An UNIX socket version */

#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <iostream.h>

#if defined(__svr4__)
// For Solaris...
#include <stropts.h>
#include <sys/filio.h>

// Not sure what this might break, but its set to this value on Linux
// and on DEC/UNIX boxes...
#define INADDR_NONE INADDR_BROADCAST
#endif /* __svr4__ */

Socket::Socket(char *addr, int nport)
{
  ip = new char[strlen(addr)+1];
  strcpy(ip, addr);
  port = nport;
  debug->put("Socket constructor: ip=%s port=%d", ip, port);  
}

Socket::~Socket()
{
  szBuffer = NULL;  
  debug->put("Socket destructor");
}

void 
Socket::connectServer()
{
  struct hostent *pHostEnt;
  struct sockaddr_in ServerAddr;
  
  szBuffer = strdup("");
  
  debug->put("Connecting to ip address: %s", ip);
  
  if(inet_addr(ip) == INADDR_NONE) {
    if((pHostEnt = gethostbyname(ip)) != NULL) {
      delete [] ip;
      ip = new char [15];
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

char *
Socket::readPacket()
{
  unsigned long nLen = 0;
  
  do
    {
      ioctl (nSocket, FIONREAD, &nLen);
      if (nLen != 0)
	{
	  char szData[1024*10];
	  long nReceived;
	  if ((nReceived = recv(nSocket,(char *)&szData,sizeof(szData)-1,0))
	      == -1)
	    error->put(ERROR_FATAL, "Connection closed: %s", strerror (errno));
	  
	  szData[nReceived] = 0;
	  
	  if (config->protocol_debug)
	    {
	      fprintf (debug->fp, ">> %s", szData);
	      fflush (debug->fp);
	    }
	  
	  // Resize the buffer
	  szBuffer = (char *) realloc(szBuffer,strlen(szData)+
				      strlen(szBuffer)+1);
	  // Copy the new data
	  strcat(szBuffer,szData);
	}
    } while (nLen != 0);
  
  // Split the data into lines
  char *szReturn = NULL;
  while ((szReturn = strchr(szBuffer,'\r')) != NULL)
    {
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
Socket::writePacket(char *szFmt, ...)
{
  va_list vl;
  char szData[1024*10];
  
  va_start (vl, szFmt);
  vsprintf (szData, szFmt, vl);
  va_end (vl);
  
  sprintf (szData, "%s\r\n", szData);
  
  if (config->protocol_debug)
    {
      fprintf (debug->fp, "<< %s", szData);
      fflush (debug->fp);
    }
  
  if (send(nSocket, szData,strlen(szData),0) == -1)
    error->put(ERROR_FATAL, "Could not send data: %s", strerror (errno));
}

#endif /* __WIN32__ */
