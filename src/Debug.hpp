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

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

/** A debugging class. All debug messages should go through this class,
    and thus maintaining consistency and possibly in future specifying
    debug levels and switching debug on/off runtime. */

class Debug
{
 public:
   /// Add a new debug message.
   void put(char *, ...);
   /// Allow printf() style formatting in DEBUG() macro.
   char *string(char *, ...);
};

extern Debug *debug;

#if 0
/// Use this macro to add all the unnecessary information to your debug messages.
# define DEBUG(x) debug->put("%-17s: debug: %s", debug->string("%s:%d", __FILE__, __LINE__), x);
#else
# define DEBUG(x)
#endif /* 0 */

#endif /* __DEBUG_HPP__ */
