#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#ifdef WIN32

#include <windows.h>

/** A socket class. Handles the connecting, disconnecting, reading
    and writing packets with the server. */
class Socket
{

public:
	Socket(char *szHost,int nPort);					// Connect to a host.
	virtual ~Socket();
	void	Thread();									// The new thread.
public:
	virtual char *readPacket();					// Reads a line of data, returns NULL if there is no line ready. (The tailing \r is removed)
	virtual void writePacket(char *szFmt,...);	// Writes data.

	void	Dispatch(char *szString);
public:
	char	*szHost;
	int		nPort;
	int		nSocket;
	char	*szBuffer;
	HANDLE	hWait;	// The wait handle.
	HANDLE	NetThread;
	HANDLE	LinesSemaphore;	// We need a semaphore to be sure that the linked list of lines recived is ok.
	struct LinkedList_s {
		char *szText;
		struct LinkedList_s *pNext;
	};
	struct LinkedList_s *pFirstLine;
};

#else

class Socket
{
 public:
   Socket();
   Socket(char *ip, int port);
   ~Socket();
   /// Connect to the server.
   void connectServer();
   /// Disconnect the server.
   void disconnectServer();
   /** Read the packet sent by the server if there is any.
       @returns The packet sent by the server. */
   char *readPacket();
   /// Write packet to the server.
   void writePacket(char *buf, ...);
 public:
   /// IP address if the server.
   char *ip;
   /// Port of the server.
   int port;
 private:
   int nSocket;
   char *szBuffer;

};


#endif /* __WIN32__ */

extern Socket *sock;

#endif /* __SOCKET_HPP__ */

