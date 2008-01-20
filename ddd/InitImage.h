// $Id$ -*- C++ -*-
// Create an XImage from bitmap data

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_InitImage_h
#define _DDD_InitImage_h

#ifdef IF_MOTIF

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#endif // IF_MOTIF

#include "gtk_wrapper.h"

// Initialize IMAGE
extern void InitImage(XIMAGE_P image);

// Create IMAGE from bitmap source
extern XIMAGE_P CreateImageFromBitmapData(unsigned char *bits,
					  int width, int height);

// Install IMAGE in Motif cache
#ifdef IF_MOTIF
Boolean InstallImage(XIMAGE_P image, const char *name);
#else // NOT IF_MOTIF
Boolean InstallImage(XIMAGE_P image, XIMAGE_P &pointer);
#endif // IF_MOTIF

// Install bitmap in Motif cache
#ifdef IF_MOTIF
Boolean InstallBitmap(unsigned char *bits, int width, int height, const char *name);
#else // NOT IF_MOTIF
Boolean InstallBitmap(unsigned char *bits, int width, int height, XIMAGE_P &name);
#endif // IF_MOTIF

#endif // _DDD_InitImage_h
// DON'T ADD ANYTHING BEHIND THIS #endif
