#ifndef __DEBUG_H__
#define __DEBUG_H__

class Debug
{
 public:
   Debug();
   ~Debug();
   void put(const char *, ...);
 private:
   char *buf;
};

#endif