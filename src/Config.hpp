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

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

/** A configuration class. This class is for reading and possibly in
    future saving the majikrc/majik3d.ini configuration files. It could
    however also contain all the configuration variables and thus
    encapsulating the whole scheme to this class. */

class Config
{
public:
  Config();
  ~Config();
  /// Try to read user configuration from configuration files.
  void readConfig();
private:
  void parseLine(char *strbuf, int line);
  void parseOption(char *option, char *value);
  bool readOptions(char *filename);
public:
  char *server_ip;
  int server_port;
  int debug_level, protocol_debug;
  ///
  int screen_width;
  ///
  int screen_height;
  ///
  int bpp;
  ///
  int mousetrap;
  ///
  int nomousetrap;
  ///
  int noTexture;
  ///
  int wireframe;
  ///
  int nomouse;
  ///
  int nofog;
  ///
  int nosmooth;
  ///
  int nomenu;
  ///
  int camera;
  ///
  int fullscreen;
  ///
};

extern Config *config;

#endif /* __CONFIG_HPP__ */
