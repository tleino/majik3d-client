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

#include <GL/gl.h>
#include <pu.h>
#include <sys/time.h>

#include "mcDebug.hpp"
#include "mcOverlay.hpp"
#include "mcProtocol.hpp"
#include "mcInput.hpp"
#include "mcDisplay.hpp"
#include "mcSocket.hpp"
#include "mcScene.hpp"
#include "mcConfig.hpp"
#include "mcMenu.hpp"

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
  
  status_text = new puText (5, 5);
  status_text->setColour (PUCOL_LABEL, 1.0, 1.0, 1.0);
  status_text->setLabel ("[status_text]");
  
  inp = new puInput ( 5, 25, display->width-5, 5+20 ) ;
  inp->setLegend    ( "Legend" ) ;
  inp->setValue ("");
  inp->setLabel ("");
  inp->acceptInput  () ;
  inp->setCursor ( 0 ) ;
  inp->setCallback (Input::promptDown);
  inp->hide();
  
  menu->init();
}

// FIXME: Needs to be cleaned.

double start_time = 0;
time_t t = time(NULL);
int frames = 0;
char fps_text[80];

double read_time_of_day(); 
 
double read_time_of_day () 
{ 
#ifdef WIN32 
  _int64 u, v ; 
  QueryPerformanceCounter   ((LARGE_INTEGER*) &u ) ; 
  QueryPerformanceFrequency ((LARGE_INTEGER*) &v ) ; 
  return (double)u / (double)v ; 
#else 
  timeval tv ; 
  gettimeofday ( &tv, NULL ) ; 
   
  return (double) tv . tv_sec + (double) tv . tv_usec / 1000000.0 ; 
#endif 
} 

void
Overlay::draw()
{
  frames++;

  if (inp_command != CMD_LOGIN)
    {
      sprintf (fps_text, "%1.1f", (float) 1/(read_time_of_day() - start_time));
      start_time = read_time_of_day();
      // calculate fps
      status_text->setLabel(fps_text);
    }

  glEnable (GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glAlphaFunc(GL_GREATER, 0.1f);
  puDisplay();
  glDisable (GL_BLEND);
}
