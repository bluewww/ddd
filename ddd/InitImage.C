// $Id$ -*- C++ -*-
// Create an XImage from bitmap data

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char InitImage_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "InitImage.h"

#include "config.h"
#include "assert.h"

#include <sys/types.h>
#include <X11/Xlib.h>

#include <X11/Xlibint.h>	// Xcalloc()
#include <X11/Xutil.h>		// XGetPixel(), etc.

#if XlibSpecificationRelease < 6

// We're stuck with X11R5 or earlier, so we Provide a simple
// XInitImage() replacement.  These GetPixel, PutPixel, and SubImage
// functions only work for Bitmap data.
// 
// These functions were adapted from X11R6 `ImUtil.c', for which the
// following copyright applies:
//
// Copyright (c) 1986 X Consortium
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT.  IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name of the X Consortium
// shall not be used in advertising or otherwise to promote the sale,
// use or other dealings in this Software without prior written
// authorization from the X Consortium.


// The ROUNDUP macro rounds up a quantity to the specified boundary,
// then truncates to bytes.
#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad)-1)) / (pad)) * ((pad)>>3))

// Read a pixel from an 1-bit image data structure
static unsigned long GetPixel (XImage *image, int x, int y)
{
    int xoff = x;
    int yoff = y * image->bytes_per_line + (xoff >> 3);
    xoff &= 7;
    unsigned char bit = 1 << xoff;
    return (image->data[yoff] & bit) ? 1 : 0;
}

// Write a pixel into an 1-bit image data structure
static int PutPixel (XImage *image, int x, int y, unsigned long pixel)
{

    int xoff = x;
    int yoff = y * image->bytes_per_line + (xoff >> 3);
    xoff &= 7;
    unsigned char bit = 1 << xoff;
    if (pixel & 1)
	image->data[yoff] |= bit;
    else
	image->data[yoff] &= ~bit;

    return 1;
}

// Clone a new (sub)image from an existing one
static XImage *SubImage (XImage *image, int x, int y, 
			 unsigned int width, unsigned int height)
{
    XImage *subimage = (XImage *) Xcalloc (1, sizeof (XImage));
    if (subimage == 0)
	return subimage;

    subimage->width            = width;
    subimage->height           = height;
    subimage->xoffset          = 0;
    subimage->format           = image->format;
    subimage->byte_order       = image->byte_order;
    subimage->bitmap_unit      = image->bitmap_unit;
    subimage->bitmap_bit_order = image->bitmap_bit_order;
    subimage->bitmap_pad       = image->bitmap_pad;
    subimage->bits_per_pixel   = image->bits_per_pixel;
    subimage->depth            = image->depth;
    subimage->bytes_per_line   = ROUNDUP(width, subimage->bitmap_pad);
    subimage->obdata           = 0;

    InitImage(subimage);

    unsigned int dsize = subimage->bytes_per_line * height;

    char *data = (char *)Xcalloc (1, dsize);
    if (data == 0)
    {
	Xfree((char *) subimage);
	return 0;
    }
    subimage->data = data;

    if (int(height) > image->height - y)
	height = image->height - y;
    if (int(width) > image->width - x)
	width = image->width - x;

    for (unsigned int row = y; row < (y + height); row++)
    {
	for (unsigned int col = x; col < (x + width); col++)
	{
	    unsigned long pixel = XGetPixel(image, col, row);
	    XPutPixel(subimage, (col - x), (row - y), pixel);
	}
    }

    return subimage;
}

#endif // XlibSpecificationRelease



void InitImage(XImage *image)
{
#if XlibSpecificationRelease >= 6
    XInitImage(image);
#else
    assert(image->xoffset          == 0);
    assert(image->format           == XYBitmap);
    assert(image->byte_order       == MSBFirst);
    assert(image->bitmap_unit      == 8);
    assert(image->bitmap_bit_order == LSBFirst);
    assert(image->bitmap_pad       == 8);
    assert(image->depth            == 1);
    assert(image->bytes_per_line   == (image->width + 7) / 8);

    image->f.get_pixel = GetPixel;
    image->f.put_pixel = PutPixel;
    image->f.sub_image = SubImage;
#endif
}

XImage *CreateImageFromBitmapData(unsigned char *bits, int width, int height)
{
    XImage *image = (XImage *)Xcalloc(1, sizeof(XImage));
    image->width            = width;
    image->height           = height;
    image->xoffset          = 0;
    image->format           = XYBitmap;
    image->data             = (char *) bits;
    image->byte_order       = MSBFirst;
    image->bitmap_unit      = 8;
    image->bitmap_bit_order = LSBFirst;
    image->bitmap_pad       = 8;
    image->depth            = 1;
    image->bytes_per_line   = (width + 7) / 8;

    InitImage(image);

    return image;
}
