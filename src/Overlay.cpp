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
   
   sock->writePacket ("%d %s", overlay->inp_command, val);
   overlay->inp->setValue (" "); // PLIB FIXME: Segfault if set to "" !!
   overlay->inp->setLabel ("");
   overlay->inp->acceptInput  () ;
   overlay->inp->setCursor ( 0 ) ;
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
   
   inp = new puInput ( 5, 25, display->width-5, 5+20 ) ;
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
