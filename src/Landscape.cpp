#include <stdio.h>
#include <stdlib.h>
#include "Landscape.hpp"
#include "Object.hpp"
#include "Majik.hpp"

Landscape::Landscape()
{
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
   Object *newob = new Object;
   newob->id = oid;
   newob->next = first;
   first = newob;
}

void Landscape::removeObject(int oid)
{
   Object **obj = &first;
   while(*obj != NULL)
	 {
		if((*obj)->id == oid)
		  {
			 Object *now = *obj;
			 *obj = now->next;
			 free(now);
			 continue;
		  }
		obj = &((*obj)->next);
	 }
}
