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

#include <GL/gl.h>
#include <pu.h>

#include "Debug.hpp"
#include "Overlay.hpp"
#include "Protocol.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "Socket.hpp"
#include "Scene.hpp"
#include "Config.hpp"
#include "Menu.hpp"

void inputCB (puObject *o)
{
   char *val = NULL;
   o->getValue (&val);
   debug->put ("got val: %s", val);
   if (strlen(val) < 3) {
	  return;
   }
   
   if (scene->initialized == 3) {
	  sgVec4 skycol ; sgSetVec4 ( skycol, 0.4f, 0.7f, 1.0f, 1.0f ) ;
	  glClearColor ( skycol[0], skycol[1], skycol[2], skycol[3] ) ;
	  
	  scene->initialized = 4;
	  sock->writePacket ("%d %s", overlay->inp_command, val);
	  overlay->inp_command = CMD_SAY;
	  o->setSize(display->width-5, 5+20);
	  if (config->nomenu == 0)
		menu->menuBar->reveal();
	  
	  memset (display->stxt, 0, sizeof(display->stxt));
	  overlay->status_text->setPosition(5, 10);
   } else {
	  sock->writePacket ("%d %s", overlay->inp_command, val);
   }
   delete overlay->inp;
   overlay->inp = new puInput ( 5, 5, display->width-5, 5+20 ) ;
   overlay->inp->setLegend    ( "Legend" ) ;
   overlay->inp->setValue (" ");
   overlay->inp->setLabel ("");
   overlay->inp->acceptInput  () ;
   overlay->inp->setCursor ( 0 ) ;
   overlay->inp->setCallback (inputCB);
   overlay->inp->hide();
}

Overlay::Overlay()
{
}

Overlay::~Overlay()
{
}

void
Overlay::init()
{
   puInit();
   if (config->nomouse != 1)
	 puShowCursor();
   
   puSetDefaultStyle (PUSTYLE_SMALL_SHADED);
   puSetDefaultColourScheme (1.0, 1.0, 1.0, 0.6f);
   
   status_text = new puText (5, 10);
   status_text->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
   
   inp = new puInput ( 5, 5, display->width-5, 5+20 ) ;
   inp->setLegend    ( "Legend" ) ;
   inp->setValue ("");
   inp->setLabel ("");
   inp->acceptInput  () ;
   inp->setCursor ( 0 ) ;
   inp->setCallback (inputCB);
   inp->hide();
   
   menu->init();
}

void
Overlay::draw()
{
   glEnable (GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glAlphaFunc(GL_GREATER, 0.1f);
   puDisplay();
   glDisable (GL_BLEND);
}
