#include <stdio.h>
#include "Object.hpp"

Object::Object()
{
   Object::lastId++;
   id = Object::lastId;
   next = NULL;
}

Object::~Object()
{
}