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
#include <netdb.h>
#include "Majik.hpp"

Config::Config()
{
   server_ip = NULL;
   server_port = 0;
   screen_width = screen_height = bpp = 0;
   
   #ifdef DEBUG
	 debug->put("Config constructor");
   #endif
}

Config::~Config()
{
   if(server_ip != NULL)
	 delete [] server_ip;
   
   #ifdef DEBUG
	 debug->put("Config destructor");
   #endif
}

void
Config::parseOption(char *option, char *value)
{
   struct hostent *h;
   
   if(strcmp(option, "server") == 0)
	 {
		debug->put("Ja valuehan oli: %s", value);
		if(sscanf(value, "%*d.%*d.%*d.%*d") == 4)
		  {
			 server_ip = new char[16];
			 strcpy(&server_ip[0], &value[0]);
		  }
		else
		  {
			 h = gethostbyname(value);
			 if(h == NULL)
			   error->put(ERROR_FATAL, "Invalid host in majikrc.");
			 sprintf(&server_ip[0], "%d.%d.%d.%d",
					 (unsigned char)h->h_addr_list[0][0],
					 (unsigned char)h->h_addr_list[0][1],
					 (unsigned char)h->h_addr_list[0][2],
					 (unsigned char)h->h_addr_list[0][3]);
		  }
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
   
   // This works also with '      option = value' lines
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
   
   for(j=i;j<strlen(strbuf);j++)
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
   
   debug->put("option: '%s' = '%s'\n", option, value);
   parseOption(&option[0], &value[0]);
}

void
Config::readOptions(char *filename)
{
   FILE *fp;
   char *strbuf;
   char *homedir;
   char file[80];
   int line;
   
   if(filename[0] == '~' && filename[1] == '/')
	 {
		homedir = getenv("HOME");
		sprintf(file, "%s/%s", homedir, filename);
	 }
   
   sprintf(file, "%s", filename);
   
   fp = fopen(file, "r");
   if(fp == NULL)
	 {
		return;
	 }
   
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
}
