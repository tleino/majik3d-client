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

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
# include <windows.h>
# include <time.h>
#else
# include <unistd.h>
#endif
#include <pu.h>
#include <ssg.h>

#include "mcMenu.hpp"
#include "mcInput.hpp"
#include "mcDebug.hpp"
#include "mcDisplay.hpp"
#include "mcScene.hpp"
#include "mcSocket.hpp"
#include "mcProtocol.hpp"
#include "mcConfig.hpp"
#include "mcOverlay.hpp"
#include "mcMapquad.hpp"

int mouse_x, mouse_y;
extern Player *tuxi;

bool Display::sceneVisible = false;

Display::Display()
{
  m_width = 640;
  m_height = 480;
  m_initialWidth = m_width;
  m_initialHeight = m_height;
  m_bpp = 16;
  
  debug->put("Display constructor");
}

Display::Display(int w, int h, int b)
{
  m_width = w;
  m_height = h;
  m_initialWidth = w;
  m_initialHeight = h; 
  m_bpp = b;
  
  debug->put("Display constructor: width=%d height=%d bpp=%d", m_width, m_height,
	     m_bpp);
}

Display::~Display()
{
  closeScreen();
  
  debug->put("Display destructor");
}

void
Display::openScreen()
{  
  debug->put("Opening screen...");
  if (config->testFlag(mcConfig::MOUSE_TRAP))
	  config->setFlag ( mcConfig::MOUSE_TRAP, 1);
  
//  config->setCameraMode(0);
//  display->setPitch(0);
  
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  if (0)
	  ;
//  if (config->gamemode)
//    glutEnterGameMode();
  else
    {
      glutInitWindowSize(m_width, m_height);
      glutInitWindowPosition(0, 0);
      glutCreateWindow("majik");
    }
  
  glutReshapeFunc(resizeScreen);
  glutDisplayFunc(updateScreen);
  glutKeyboardFunc(input->keyDown);
  glutSpecialFunc(input->specialDown);
 // glutSpecialUpFunc(input->specialUp);
  glutMouseFunc(input->mouseDown);
  glutPassiveMotionFunc(input->mouseMotion);
  glutMotionFunc(input->mouseMotion);

//  glutIgnoreKeyRepeat(1);
  
  if (config->testFlag(mcConfig::MOUSE_TRAP) )
    glutWarpPointer (m_width/2, m_height/2);
  
  /* PLIB: Simple Scene Graph */
  debug->put("ssgInit");
  ssgInit();
  
  overlay->init();
  scene->init();
  
  glEnable (GL_DEPTH_TEST);
  
  if (!config->testFlag(mcConfig::FOG))
    glDisable (GL_FOG);
  if (config->testFlag(mcConfig::SMOOTH))
    glShadeModel (GL_SMOOTH);
  else
    glShadeModel (GL_FLAT);
  
  debug->put("Screen opened.");
}

void 
Display::closeScreen()
{
  debug->put("Closing screen...");
  debug->put("Screen closed.");
}

bool mapFound = true;

void
Display::updateScreen()
{
  char *tmp = 0;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
//  tmp = sock->readPacket();
  
  while (tmp != NULL)
    {
      protocol->parseCommand(tmp);
      tmp = sock->readPacket();
    }
  
  //   if (mousetrap)
  //	 mousetrap();
  //   if (Display::sceneVisible)
 
	if (tuxi == NULL)
	{
		protocol->parseCommand("52 1");
		protocol->parseCommand("55 1 1000 1000 0 dog3.ac");
	}

  if (mapFound)
    {
      scene->draw();
    }
  else if (tuxi != NULL)
    {
      Mapquad *temp;
      sgCoord tuxiPos = tuxi->getPos();

      temp = Mapquad::root_map->tryMapquad(12,
					   tuxiPos.xyz[0],
					   tuxiPos.xyz[1]);
      if (temp != NULL && temp->isMapReceived())  
	mapFound = true;
    }

  overlay->draw();
  
  glutSwapBuffers();
  glutPostRedisplay();
}

void
Display::resizeScreen(int w,int h)
{
  display->setWidth(w);
  display->setHeight(h);
  glViewport(0, 0, w, h);
  overlay->inp->setSize(display->getWidth()-5, 5+20 ) ;
}
