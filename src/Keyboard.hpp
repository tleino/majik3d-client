#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

class Keyboard
{
 public:
   Keyboard();
   ~Keyboard();
   void keyDown(unsigned char, int, int);
   void specialDown(int, int, int);
};

#endif
