/* Majik 3D client
 * Copyright (C) 1999  Majik Development Team <majik@majik3d.org>
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

#ifndef __OVERLAY_HPP__
#define __OVERLAY_HPP__

#include <pu.h>

/** An overlay class. This class draws all "overlaying" stuff to the
    screen after others has been done. This includes drawing menus, fps
    calculator, prompts, dialogs etc. */

class Overlay
{
public:
  Overlay();
  ~Overlay();
  
  /// Initialize the class.
  void init();
  /// Draw the stuff to the screen.
  void draw();
  
  /// Pointer to status bar.
  puText *status_text;
  /// Pointer to input prompt.
  puInput *inp;
  /// The command executed after input prompt has received '\n'.
  int inp_command;
};

extern Overlay *overlay;

#endif /* __OVERLAY_HPP__ */
