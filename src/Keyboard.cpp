#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Keyboard::Keyboard()
{
   #ifdef DEBUG
	 debug->put("Keyboard constructor");
   #endif
}

Keyboard::~Keyboard()
{
   #ifdef DEBUG
	 debug->put("Keyboard destructor");
   #endif
}

void
Keyboard::keyDown(unsigned char key, int x, int y)
{
   switch (key) {
	default:
	  error->put(ERROR_WARNING, "Unsupported key received: %d at (%d,%d)", key, x, y);
	  break;
   }
   
   glutPostRedisplay();
}

void 
Keyboard::specialDown(int key, int x, int y)
{
   switch(key) {
	case GLUT_KEY_LEFT:
	  landscape->angle -= 2;
	  break;
	case GLUT_KEY_UP:
	  break;
	case GLUT_KEY_RIGHT:
	  landscape->angle += 2;
	  break;
	case GLUT_KEY_DOWN:
	  break;
	case GLUT_KEY_PAGE_UP:
	  break;
	case GLUT_KEY_PAGE_DOWN:
	  break;
	case GLUT_KEY_HOME:
	  landscape->angle = 0;
	  break;
	case GLUT_KEY_END:
	  break;
	default:
	  error->put(ERROR_WARNING, "Unsupported special key received: %d at (%d,%d)", key, x, y);
	  break;
   }

   glutPostRedisplay();
}
