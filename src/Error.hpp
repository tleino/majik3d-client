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

#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include <errno.h>

/// Highest severity of an error; exits the program.
#define ERROR_FATAL   -1
/// Lowest severity of an error; displays the error and continues.
#define ERROR_WARNING  1

/** An error message handling class. All the error messages should
    go through this class to ensure consistency and handling of
    different severity levels and what to do with them. */

class Error
{
public:
  Error();
  ~Error();
  /// Adds a new error message.
  void put(const int severity, const char *error_message, ...);
private:
  char *buf;
};

extern Error *error;

#endif /* __ERROR_HPP__ */
