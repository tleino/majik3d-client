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

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

/** A configuration class. This class is for reading and possibly in
    future saving the majikrc/majik3d.ini configuration files. It could
    however also contain all the configuration variables and thus
    encapsulating the whole scheme to this class. */

class mcConfig
{
public:

	enum e_flag
	{
		FULLSCREEN		= (0x1 << 0),
		WIREFRAME		= (0x1 << 1),
		MOUSE_TRAP		= (0x1 << 2),
		MENU			= (0x1 << 3),
		FOG				= (0x1 << 4),
		TEXTURED		= (0x1 << 5),
		SMOOTH			= (0x1 << 6),
		PROTOCOL_DEBUG	= (0x1 << 7),
		MOUSE			= (0x1 << 8)
	};

			mcConfig			();
			~mcConfig			();
  
	/// Try to read user configuration from configuration files.
	void	readConfig		();

	void	setFlag			(e_flag flag, bool set)
	{
		if (set)
			m_flags |= flag;
		else
			m_flags &= ((1>>1) ^ flag);
	}

	bool	testFlag		(e_flag flag)
	{
		return (m_flags & flag) != 0;
	}


	int		getScreenWidth	()	{ return m_screenWidth;	}
	int		getScreenHeight	()	{ return m_screenHeight; }
	int		getScreenBpp	()	{ return m_screenBpp; }
	int		getDebugLevel	()	{ return m_debugLevel; }
	int		getCameraMode	()	{ return m_cameraMode; }

	void	setCameraMode	(int m) { m_cameraMode = m; }

private:
	void	parseLine	(char *strbuf, int line);
	void	parseOption	(char *option, char *value);
	bool	readOptions	(char *filename);

	unsigned int	m_flags;

	char *m_serverIP;
	int	m_serverPort;
	int	m_debugLevel;
	int	m_screenWidth;
	int	m_screenHeight;
	int	m_screenBpp;
	int	m_cameraMode;
	int	m_gameMode;
	int	m_lod;
};

extern mcConfig *config;

#endif /* __CONFIG_HPP__ */
