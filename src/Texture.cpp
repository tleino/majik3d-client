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

#include <png.h>
#include <pngconf.h>
#include <errno.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "Texture.hpp"
#include "Debug.hpp"

unsigned int width, height, pitch;
void *pixels;

Texture::Texture()
{
   glGenTextures(1, &textureId);
   DEBUG(debug->string("Texture constructor: textureId=%d", textureId));
}

Texture::~Texture()
{
   DEBUG ("Texture destructor.");
}

void
Texture::loadTexture(char *fname)
{
   loadPNG (fname);
}

void *
Texture::getPixels()
{
   return pixels;
}

void
Texture::loadPNG (char *file_name) {
   FILE *fp = fopen(file_name, "rb");
   int is_png, row;
   png_bytep header;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL, end_info = NULL;
   png_bytep *row_pointers = NULL;
      
   /* Check if the file exists and is in PNG-format */
   if (!fp)
	 {
		return;
	 }
   fread(header, 1, 0, fp);
   is_png = !png_sig_cmp(header, 0, 0);
   if (!is_png)
	 {
		return;
	 }
   
   /* Allocate internal structures */
   png_ptr = png_create_read_struct
	 (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) {
	 return;
   }
   
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
	 {
		png_destroy_read_struct(&png_ptr,
								(png_infopp)NULL, (png_infopp)NULL);
		return;
	 }
   
   end_info = png_create_info_struct(png_ptr);
   if (!end_info)
	 {
		png_destroy_read_struct(&png_ptr, &info_ptr,
								(png_infopp)NULL);
		return;
	 }
   
   /* Clean up the structures if an error is caught */
   if (setjmp(png_ptr->jmpbuf))
	 {
		png_destroy_read_struct(&png_ptr, &info_ptr,
								&end_info);
		fclose(fp);
		return;
	 }
   
   /* Feed our fp to PNG's internal kludge system */
   png_init_io(png_ptr, fp);
   
   /* We have read the file a little bit already, so now we must seek back
	* to the beginning */
   png_set_sig_bytes(png_ptr, 0);
      
   /* Read all the available info from the image to info_ptr */
   png_read_info(png_ptr, info_ptr);
   height = png_get_image_height(png_ptr, info_ptr);
   width = png_get_image_width(png_ptr, info_ptr);
   pitch = png_get_rowbytes(png_ptr, info_ptr);
   pixels = (void *) malloc(width * pitch);
   row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
   
   if (pixels == NULL || row_pointers == NULL) {
	  printf ("out of memory.\n");
	  return;
   }
   
   DEBUG (debug->string("%s (width=%d height=%d)", file_name, width, height));
   
   /* Strip the image to 8bpp because our code might not support 16bpp
	* textures, I think. FIXME: Verify */
   png_set_strip_16(png_ptr);

   /* Read the image, now for real. Pixels are stored in th erow_pointers
	* array */
   for (row = 0; row < height; row++) {
	  row_pointers[row] = (png_bytep) (unsigned short *) pixels + row*pitch;
   }
   
   png_read_image(png_ptr, row_pointers);
}
