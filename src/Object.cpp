#include <stdio.h>
#include "Object.hpp"

Object::Object()
{
   Object::lastId++;
   id = Object::lastId;
   next = NULL;
   #ifdef DEBUG
	 Debug("Object constructor: id=%d", id);
   #endif
}

Object::~Object()
{
}