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

#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include <stdio.h>
#include <plib/ssg.h>

struct Material {
 public:
   ssgSimpleState **gst;
   
   char *texture_map;
   int clamp_tex;
   int transparency;
   float alpha_ref;
   int lighting;
   float friction;
   
   int isNull() { return gst == NULL; };
   void install(int index);
   
   ssgState *getState() { return *gst; }
   char *getTexFname() { return texture_map; }
};
 
void initMaterials();
Material *getMaterial(ssgState *s);
Material *getMaterial(ssgLeaf *l);

extern ssgSimpleState *grass_gst, *sandstone_gst, *ukkelipukkeli_gst;

#endif /* __MATERIAL_HPP__ */
