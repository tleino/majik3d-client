#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Keyboard::Keyboard()
{
   #ifdef DEBUG
	 Debug("Keyboard constructor");
   #endif
}

Keyboard::~Keyboard()
{
   #ifdef DEBUG
	 Debug("Keyboard destructor");
   #endif
}

void 
Keyboard::keyPressed(unsigned char c)
{
   switch(c)
	 {
	  case 'q':
	  case 'Q':
		quit = true;
		break;
	 }
}
