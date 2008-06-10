// $Id$ -*- C++ -*-
// Create an XImage from bitmap data

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_InitImage_h
#define _DDD_InitImage_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#else
#include <GUI/Image.h>
#endif

#if defined(IF_XM)
// Initialize IMAGE
extern void InitImage(XImage *image);

// Create IMAGE from bitmap source
extern XImage *CreateImageFromBitmapData(unsigned char *bits,
					 int width, int height);

// Install IMAGE in Motif cache
Boolean InstallImage(XImage *image, const char *name);

// Install bitmap in Motif cache
Boolean InstallBitmap(unsigned char *bits, int width, int height, const char *name);
#else
// Initialize IMAGE
extern void InitImage(GUI::ImageHandle &image);

// Create IMAGE from bitmap source
extern GUI::ImageHandle CreateImageFromBitmapData(unsigned char *bits,
						  int width, int height);

// Install IMAGE in Image cache
bool InstallImage(GUI::ImageHandle image, GUI::ImageHandle &pointer);

// Install bitmap in Image cache
bool InstallBitmap(unsigned char *bits, int width, int height, GUI::ImageHandle &name);
#endif

#endif // _DDD_InitImage_h
// DON'T ADD ANYTHING BEHIND THIS #endif
