#include <stdio.h>
#include <stdlib.h>
#include "Majik.hpp"

Object::Object()
{
   Object::lastId++;
   id = Object::lastId;
   next = NULL;
   #ifdef DEBUG
	 debug->put("Object constructor: id=%d", id);
   #endif
}

Object::~Object()
{
   if(next != NULL)
	 {
		delete next;
		next = NULL;
	 }
   #ifdef DEBUG
	 debug->put("Object destructor: id=%d", id);
   #endif
}