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

#include "Error.hpp"
#include "Debug.hpp"
#include "Config.hpp"
#include "Menu.hpp"
#include "Display.hpp"

Config::Config()
{
  screen_width = 0;
  screen_height = 0;
  bpp = 0;
  mousetrap = 0;
  nomousetrap = 0;
  noTexture = 0;
  wireframe = 0;
  nomouse = 0;
  nofog = 0;
  nosmooth = 0;
  nomenu = 0;
  camera = 0;
  fullscreen = 0;
  server_ip = NULL;
  server_port = 0;
  debug_level = 0;
  protocol_debug = 0;
  lod = 7;
}

Config::~Config()
{
}

void
Config::parseOption(char *option, char *value)
{   
  if(strcmp(option, "server") == 0)
    {
      server_ip = new char [strlen(value)+1];
      strcpy(server_ip, value);
      return;
    }
  if(strcmp(option, "port") == 0)
    {
      sscanf(value, "%d", &server_port);
      return;
    }
  if(strcmp(option, "screen_width") == 0)
    {
      sscanf(value, "%d", &screen_width);
      return;
    }
  if(strcmp(option, "screen_height") == 0)
    {
      sscanf(value, "%d", &screen_height);
      return;
    }
  if(strcmp(option, "bpp") == 0)
    {
      sscanf(value, "%d", &bpp);
      return;
    }
  if(strcmp(option, "notexture") == 0)
    {
      if (atoi(value) == 1)
	menu->noTextureCB(NULL);
    }
  if (strcmp(option, "nomousetrap") == 0)
    {
      if (atoi(value) == 1)
	menu->noMousetrapCB(NULL);
      else
	menu->mousetrapCB(NULL);
    }
  if (strcmp(option, "nomouse") == 0)
    {
      if (atoi(value) == 1)
	menu->noMouseCB(NULL);
    }
  if (strcmp(option, "nosmooth") == 0)
    {
      this->nosmooth = atoi(value);
    }
  if (strcmp(option, "nofog") == 0)
    {
      if (atoi(value) == 1)
	menu->noFogCB(NULL);
    }
  if (strcmp(option, "nomenu") == 0)
    {
      this->nomenu = atoi (value);
    }
  if (strcmp(option, "camera") == 0)
    {
      this->camera = atoi(value);
    }
  if (strcmp(option, "fullscreen") == 0)
    {
      if (atoi(value) == 1)
	this->fullscreen = 2;
      else
	this->fullscreen = 0;
    }
  if (strcmp(option, "debug") == 0)
    {
      debug_level = atoi(value);
    }
  if (strcmp(option, "protocol_debug") == 0)
    {
      protocol_debug = atoi(value);
    }
  if (strcmp(option, "lod") == 0)
    {
      lod = atoi(value);
    }

}

void
Config::parseLine(char *strbuf, int line)
{
  int i, j;
  int ostart, oend, vstart, vend;
  char option[32];
  char value[128];
  
  memset(&option[0], 0, 32);
  memset(&value[0], 0, 128);
  
  // This works also with '      option=value' lines
  ostart = 0;
  while(strbuf[ostart] == ' ')
    ostart++;
  
  for(i=ostart;i<ostart+32;i++)
    {
      if(strbuf[i] == '=')
	break;
      if(strbuf[i] == '\n')
	return;
    }
  
  oend = i;
  if(i == 32)
    {
      error->put(ERROR_WARNING, "majikrc: parse error at line #%d\n", line);
      return;
    }
  
  for(j=i;j<(signed)strlen(strbuf);j++)
    if(strbuf[j] == '\n')
      break;
  
  for(i=ostart;i<oend;i++)
    if(strbuf[i] != ' ')
      option[i - ostart] = strbuf[i];
  
  vstart = oend;
  vstart++;
  
  while(strbuf[vstart] == ' ')
    vstart++;
  
  vend = vstart;
  while(strbuf[vend] != ' ' && strbuf[vend] != '\n')
    vend++;
  
  memcpy(&value[0], &strbuf[vstart], vend - vstart);
  
  // debug->put("option: '%s' = '%s'", option, value));
  parseOption(&option[0], &value[0]);
}

bool
Config::readOptions(char *filename)
{
  FILE *fp;
  char *strbuf;
  char file[80];
  int line;
  
  if(filename[0] == '~' && filename[1] == '/')
    sprintf(file, "%s/%s", getenv("HOME"), ".majikrc");
  else
    strcpy(file, filename);
  
  fp = fopen(file, "r");
  if(fp == NULL)
    return false;
  
  strbuf = new char [128+32];
  line = 0;
  
  while(!feof(fp))
    {
      if(fgets(strbuf, 128+32, fp) == NULL)
	break;
      line++;
      if(strbuf[0] == '#')
	continue;
      parseLine(strbuf, line);
    }
  delete strbuf;
  return true;
}

void
Config::readConfig ()
{
  if(config->readOptions("~/.majikrc") == false)
    if(config->readOptions("/etc/majikrc") == false)
      if(config->readOptions("majikrc") == false)
	if(config->readOptions("majik3d.ini") == false)
	  error->put(ERROR_WARNING, "No majikrc or majik3d.ini file found.");
}