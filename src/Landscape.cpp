#include <stdio.h>
#include <stdlib.h>
#include "Landscape.hpp"
#include "Object.hpp"
#include "Majik.hpp"

Landscape::Landscape()
{
   first = NULL;
   #ifdef DEBUG
	 Debug("Landscape constructor");
   #endif
}

Landscape::~Landscape()
{
   if(first != NULL)
	 free(first);
   #ifdef DEBUG
	 Debug("Landscape destructor");
   #endif
}

void Landscape::addObject(int oid)
{
}

void Landscape::removeObject(int oid)
{
}
