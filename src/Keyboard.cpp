#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
}

void Keyboard::keyPressed(unsigned char c)
{
   switch(c)
	 {
	  case 'q':
	  case 'Q':
		quit = true;
		break;
	 }
}
