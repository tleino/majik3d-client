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
#include <iostream.h>
#include <stdarg.h>
#include <stdlib.h>

#include "Error.hpp"

Error::Error()
{
  buf = new char[1024];
}

Error::~Error()
{
  delete [] buf;
}

void
Error::put(const int severity, const char *fmt, ...)
{
  va_list vl;
  va_start (vl, fmt);
  vsprintf (buf, fmt, vl);
  va_end (vl);
  
  switch(severity)
    {
    case ERROR_FATAL:
      cerr << "FATAL ERROR: ";
      break;
    case ERROR_WARNING:
      cerr << "WARNING: ";
      break;
    }
  
  cerr << buf << endl;
  
  switch (severity)
    {
    case ERROR_FATAL:
      exit (1);
      break;
    case ERROR_WARNING:
      break;
    }
}