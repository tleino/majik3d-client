#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Majik.hpp"

class Display
{
 public:
   Display();
   Display(int, int, int);
   ~Display();
   void openScreen();
   void closeScreen();
   void updateScreen();
 private:
   void idle();
 public: 
   int width, height, bpp;
};
   
#endif
