#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "Majik.hpp"

void
Debug(char *fmt, ...)
{
   va_list vl;
   static char buf[1024];
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
   cerr << buf << endl;
}
