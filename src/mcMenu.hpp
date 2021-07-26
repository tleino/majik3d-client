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

#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <plib/pu.h>

/** A menu class. This class handles the menu system. */

class Menu {
public:
  Menu();
  ~Menu();
  /// Initialize and open up the menu bar.
  void init();
  
  /// Menu exit callback.
  static void exitCB(puObject *);
  /// Switch to flat shading mode.
  static void flatCB(puObject *);
  /// Switch to smooth shading mode.
  static void smoothCB(puObject *);
  /// Enable wireframe mode.
  static void wireframeCB(puObject *);
  /// Exit the wireframe mode.
  static void noWireframeCB(puObject *);
  /// Enable fog.
  static void fogCB(puObject *);
  /// Disable fog.
  static void noFogCB(puObject *);
  /// Enable mouse trap.
  static void mousetrapCB(puObject *);
  /// Disable mouse trap.
  static void noMousetrapCB(puObject *);
  /// Enable software mouse.
  static void mouseCB(puObject *);
  /// Disable software mouse.
  static void noMouseCB(puObject *);
  /// Open up an about dialog.
  static void aboutCB(puObject *);
  /// Enable textures.
  static void textureCB(puObject *);
  /// Disable textures.
  static void noTextureCB(puObject *);
  /// Enable/Disable fullscreen mode.
  static void fullscreenCB(puObject *);
  
  /// Make dialog box.
  static void mkDialog(char *);
  /// Callback function for dialog box.
  static void mkDialogCB(puObject *);
  
  /// Pointer to the menu bar.
  puMenuBar *menuBar;
};

extern Menu *menu;

#endif /* __MENU_HPP__ */
