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

#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

/** A socket class. Handles the connecting, disconnecting, reading
    and writing packets with the server. */

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

extern Socket *sock;

#endif /* __SOCKET_HPP__ */
