#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#ifdef WIN32

#include <windows.h>

/** A socket class. Handles the connecting, disconnecting, reading
    and writing packets with the server. A win32 version. */

class Socket
{
public:
  /// Connect to a host.
  Socket(char *szHost,int nPort);
  virtual ~Socket();
  /// The new thread.
  void	Thread();
public:
  /// Reads a line of data, returns NULL if there is no line ready.
  virtual char *readPacket();
  /// Sends data to the server.
  virtual void writePacket(char *szFmt,...);
  ///
  void	Dispatch(char *szString);
public:
  ///
  char *szHost;
  ///
  int nPort;
  ///
  int nSocket;
  ///
  char *szBuffer;
  /// The wait handle.
  HANDLE hWait;
  ///
  HANDLE NetThread;
  /** We need a semaphore to be sure that the linked list of lines is
      received is ok. */
  HANDLE LinesSemaphore;
  struct LinkedList_s {
    char *szText;
    struct LinkedList_s *pNext;
  };
  struct LinkedList_s *pFirstLine;
};

#else

/** A socket class. Handles the connecting, disconnecting, reading
    and writing packets with the server. An UNIX version. */

class Socket
{
public:
  ///
  Socket();
  ///
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
