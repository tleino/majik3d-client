#include <stdio.h>
#include <iostream.h>
#include <stdarg.h>
#include "Majik.hpp"

Error::Error()
{
   buf = new char[1024];
}

Error::~Error()
{
   delete [] buf;
}

void
Error::put(const int severity, const char *fmt, ...)
{
   va_list vl;
   va_start (vl, fmt);
   vsprintf (buf, fmt, vl);
   va_end (vl);
   switch(severity)
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