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
  width = 640;
  height = 480;
  initialWidth = width;
  initialHeight = height;
  bpp = 16;
  
  debug->put("Display constructor");
}

Display::Display(int w, int h, int b)
{
  width = w;
  height = h;
  initialWidth = width;
  initialHeight = height; 
  bpp = b;
  
  debug->put("Display constructor: width=%d height=%d bpp=%d", width, height,
	     bpp);
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
  if (config->nomousetrap != 0)
    config->mousetrap = 1;
  
  config->camera = 0;
  display->pitch = 0;
  
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  if (0); //config->gamemode)
//    glutEnterGameMode();
  else
    {
      glutInitWindowSize(width, height);
      glutInitWindowPosition(0, 0);
      glutCreateWindow("majik");
    }
  
  glutReshapeFunc(resizeScreen);
  glutDisplayFunc(updateScreen);
  glutKeyboardFunc(input->keyDown);
  glutSpecialFunc(input->specialDown);
//  glutSpecialUpFunc(input->specialUp);
  glutMouseFunc(input->mouseDown);
  glutPassiveMotionFunc(input->mouseMotion);
  glutMotionFunc(input->mouseMotion);

//  glutIgnoreKeyRepeat(1);
  
  if (config->nomousetrap != 1)
    glutWarpPointer (width/2, height/2);
  
  /* PLIB: Simple Scene Graph */
  debug->put("ssgInit");
  ssgInit();
   // initMaterials();
  
  overlay->init();
  scene->init();
  
  glEnable (GL_DEPTH_TEST);
  
  if (config->nofog)
    glDisable (GL_FOG);
  if (config->nosmooth)
    glShadeModel (GL_FLAT);
  else
    glShadeModel (GL_SMOOTH);
  
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
  char *tmp;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  tmp = sock->readPacket();
  
  while (tmp != NULL)
    {
      protocol->parseCommand(tmp);
      tmp = sock->readPacket();
    }
  
  //   if (mousetrap)
  //	 mousetrap();
  //   if (Display::sceneVisible)
 
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
  display->width = w;
  display->height = h;
  glViewport(0, 0, w, h);
  overlay->inp->setSize(display->width-5, 5+20 ) ;
}
