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
