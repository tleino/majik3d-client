#ifndef __INPUT_H__
#define __INPUT_H__

class Input
{
 public:
   Input();
   ~Input();
   static void keyDown(unsigned char, int, int);
   static void specialDown(int, int, int);
   static void mousePassiveMotion(int, int);
};

#endif
