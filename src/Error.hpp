#ifndef __ERROR_H__
#define __ERROR_H__

#define ERROR_FATAL	   -1
#define ERROR_WARNING	1

class Error
{
 public:
   Error();
   ~Error();
   void put(const int, const char *, ...);
 private:
   char *buf;
};

#endif