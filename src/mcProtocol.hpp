/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik3d.org>
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

/** A protocol class. This class parses commands sent by the server. */

class Protocol
{
public:

	enum Command
	{
		CMD_MOVE			= 50,
		CMD_LOGIN			= 51,
		CMD_OWN_ID			= 52,
		CMD_QUIT			= 53,
		CMD_SAY				= 54,
		CMD_ADD_OBJECT		= 55,
		CMD_SUN_POS			= 56,
		CMD_MOVE_DIRECTION	= 57,
		CMD_MOVE_HEADING	= 58,
		CMD_MOVE_STOP		= 59,
		CMD_TURN			= 60,
		CMD_DIALOG			= 100,
		CMD_MAP				= 189,
		CMD_PROMPT			= 220,
		CMD_SAYHIDE			= 223
	};

  Protocol();
  ~Protocol();
  
  /** Parse a command sent by the server. */
  
  void parseCommand(char *buf);
};

extern Protocol *protocol;

#endif /* __PROTOCOL_HPP__ */
