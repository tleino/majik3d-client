#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Cursor.hpp"
#include "Majik.hpp"

class Display
{
 public:
   Display();
   Display(int, int, int);
   ~Display();
   void openScreen();
   void closeScreen();
   static void updateScreen();
//   static void mouseFun(int, int, int, int);
 private:
   static void idle();
 public: 
   int width, height, bpp;
   Cursor *cursor;
};
   
#endif
