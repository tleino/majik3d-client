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

#include "Cursor.hpp"
#include "Texture.hpp"
#include "Majik.hpp"

#define IMAGE_WIDTH   32
#define IMAGE_FILE    "gfx/cursor.png"

Cursor::Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor constructor");
   #endif
}

Cursor::~Cursor()
{
   #ifdef DEBUG
	 debug->put("Cursor destructor");
   #endif
}

void
Cursor::init()
{
   Texture *picture = new Texture();
   int i, k, c;
   
   tex = new GLubyte[IMAGE_WIDTH*IMAGE_WIDTH*4];
   
   picture->loadTexture(IMAGE_FILE);
   
   k = 0;
   
   for (i=0; i < IMAGE_WIDTH*IMAGE_WIDTH*4 ;)
	 {
		c = 0;
		if (*((GLubyte *)picture->pixels + k) != 255)
		  c = 1;
		tex[i+2] = (GLubyte) *((GLubyte *)picture->pixels + k++);
		tex[i+1] = (GLubyte) *((GLubyte *)picture->pixels + k++);
		tex[i] = (GLubyte) *((GLubyte *)picture->pixels + k++);
		
		i += 3;
		
		if (c)
		  tex[i++] = 255;
		else
		  tex[i++] = 0;
	 }
		
   
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   
   glGenTextures(1, &tex_id);
   glBindTexture(GL_TEXTURE_2D, tex_id);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				   GL_NEAREST);
   
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   glTexImage2D(GL_TEXTURE_2D, 0, 4, IMAGE_WIDTH,
				IMAGE_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&tex[0]);
   
}

void
Cursor::draw()
{
   glViewport(0, 0, display->width, display->height);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, display->width, display->height, 0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0.1);
   
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_DITHER);  
   glDisable(GL_LIGHTING);
   glDisable(GL_CULL_FACE);
   
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glPushMatrix();
   
   glTranslatef(x_pos, y_pos, 0.0);

   glBindTexture(GL_TEXTURE_2D, tex_id);
   
   glBegin(GL_QUADS);
	 {
		glTexCoord2f(0.0, 0.0);
		glVertex2f( 0.0, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex2f( IMAGE_WIDTH, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex2f( IMAGE_WIDTH, IMAGE_WIDTH);
		glTexCoord2f(1.0, 0.0);
		glVertex2f( 0.0, IMAGE_WIDTH);    
		
	 }
   glEnd();
   
   glPopMatrix();
   
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_DITHER);
   glDisable(GL_ALPHA_TEST);
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
}
