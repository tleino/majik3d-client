/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __INPUT_HPP__
#define __INPUT_HPP__

/** An input handling class. This class handles all the input from
    mouse to keyboard. The class should redirect all the work to
    more proper classes, this is just for catching the input. */

class Input
{
public:
  Input();
  ~Input();
  /// This method is called when a normal key-press has been detected.
  static void keyDown(unsigned char key, int x, int y);
  /** This method is called when a special key-press has been detected,
      for example when using arrow keys. */
  static void specialDown(int special_key, int x, int y);
  /// This method is called when a mouse-press has been detected.
  static void mouseDown(int button, int updown, int x, int y);
  /// This method is called when a '\n' has been detected on input prompt.
  static void promptDown(puObject *o);
};

extern Input *input;

#endif /* __INPUT_HPP__ */
