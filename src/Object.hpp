#ifndef __OBJECT_H__
#define __OBJECT_H__

class Object
{
 public:
   Object();
   ~Object();
 public:
   Object *next;
   static int lastId;
   int id;
};

#endif
