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

#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#define CMD_MOVE        50
#define CMD_LOGIN       51
#define CMD_OWN_ID      52
#define CMD_QUIT        53
#define CMD_SAY         54
#define CMD_ADD_OBJECT  55
#define CMD_DIALOG     100
#define CMD_MAP        189

class Protocol
{
 public:
   Protocol();
   ~Protocol();
   void parseCommand(char *);
};

extern Protocol *protocol;

#endif /* __PROTOCOL_HPP__ */
