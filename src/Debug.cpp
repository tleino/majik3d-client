#include <stdio.h>
#include <iostream.h>
#include <stdarg.h>
#include "Majik.hpp"

Debug::Debug()
{
   buf = new char[1024];
}

Debug::~Debug()
{
   delete [] buf;
}

void
Debug::put(const char *fmt, ...)
{
   va_list vl;
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
   cerr << buf << endl;
}
