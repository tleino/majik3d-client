#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "Majik.hpp"

void
Error (int severity, char *fmt, ...)
{
   va_list vl;
   static char buf[1024];
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
   switch (severity)
	 {
	  case ERROR_FATAL:
		cerr << "FATAL ERROR:" << endl;
		break;
	  case ERROR_WARNING:
		cerr << "WARNING:" << endl;
		break;
	 }
   
   cerr << buf << endl;
   switch (severity)
	 {
	  case ERROR_FATAL:
		exit(1);
		break;
	  case ERROR_WARNING:
		break;
	 }
}