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
#include <iostream.h>
#include <stdarg.h>

#include "Config.hpp"
#include "Debug.hpp"

Debug::Debug()
{
   put("Debug constructor");
   
   if (config->protocol_debug > 0) {
	  fp = fopen("protocol.debug", "w");
	  if (!fp) {
		 put("Couldn't open protocol.debug for write, protocol debug disabled.");
		 config->protocol_debug = 0;
	  } else {
		 put("Debugging protocol debug to protocol.debug.");
	  }
   }
}

Debug::~Debug()
{
   if (fp)
	 fclose (fp);
}

char *
Debug::string(char *fmt, ...)
{
   char *buf = new char[80]; // EORJWPEORJP
   memset (buf, 0, sizeof (buf));
   
   va_list vl;
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
  
   return buf;
}

void
Debug::put(char *fmt, ...)
{
   char *buf = new char[256];
   memset(buf, 0, sizeof(buf));
   va_list vl;
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
   
   if (config->debug_level > 0)
	 cerr << "DEBUG: " << buf << endl;
   
   delete buf;
}
