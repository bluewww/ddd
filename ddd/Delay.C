// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char Delay_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "DynArray.h"
#pragma implementation "VarArray.h"
#endif

#include "Delay.h"
#include "assert.h"
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

DEFINE_TYPE_INFO_0(_Delay);
DEFINE_TYPE_INFO_0(Delay);

#define SMALL_HOURGLASS_CURSOR

#if defined(SMALL_HOURGLASS_CURSOR) || defined(LARGE_HOURGLASS_CURSOR)
#include "time16.xbm"
#include "time16m.xbm"
#if time16_width != time16m_width || time16_height != time16m_height
#error Bad mask for time16
#endif
#endif

#ifdef LARGE_HOURGLASS_CURSOR
#include "time32.xbm"
#include "time32m.xbm"
#if time32_width != time32m_width || time32_height != time32m_height
#error Bad mask for time32
#endif
#endif

Cursor _Delay::hourglass_cache = 0;
Cursor _Delay::current_cursor = 0;

Cursor _Delay::hourglass_cursor()
{
    if (hourglass_cache != 0)
	return hourglass_cache;

    Display *display = XtDisplay(widget);

#if defined(SMALL_HOURGLASS_CURSOR) || defined(LARGE_HORGLASS_CURSOR)
    Window rootWindow = DefaultRootWindow(display);
    Screen *defaultScreen = DefaultScreenOfDisplay(display);

    char *cursor_bits             = time16_bits;
    char *cursor_mask_bits        = time16m_bits;
    unsigned int cursor_width     = time16_width;
    unsigned int cursor_height    = time16_height;
    unsigned int cursor_x_hot     = time16_x_hot;
    unsigned int cursor_y_hot     = time16_y_hot;

#ifdef LARGE_HOURGLASS_CURSOR
    // Fetch cursor shape
    unsigned int width, height;
    XQueryBestCursor(display, rootWindow, 32, 32, &width, &height);
    Boolean largeCursors = (width >= 32 && height >= 32);
    if (largeCursors)
    {
	cursor_bits      = time32_bits;
	cursor_mask_bits = time32m_bits;
	cursor_width     = time32_width;
	cursor_height    = time32_height;
	cursor_x_hot     = time32_x_hot;
	cursor_y_hot     = time32_y_hot;
    }
#endif

    Pixmap cursor_pixmap = 
	XCreateBitmapFromData(display, rootWindow, cursor_bits,
			      cursor_width, cursor_height);
    Pixmap cursor_mask_pixmap = 
	XCreateBitmapFromData(display, rootWindow, cursor_mask_bits,
			      cursor_width, cursor_height);
    
    XColor cursor_colors[2];
    cursor_colors[0].pixel = BlackPixelOfScreen(defaultScreen);
    cursor_colors[1].pixel = WhitePixelOfScreen(defaultScreen);

    XQueryColors(display, DefaultColormapOfScreen(defaultScreen), 
		 cursor_colors, 2);

    hourglass_cache = 
	XCreatePixmapCursor(display, cursor_pixmap, cursor_mask_pixmap,
			    cursor_colors, cursor_colors + 1, 
			    cursor_x_hot, cursor_y_hot);
#else // Watch cursor
    hourglass_cache = 
	XCreateFontCursor(display, XC_watch);
#endif

    return hourglass_cache;
}

_Delay::_Delay(Widget w):
    widget(w)
{
    if (widget == 0)
	return;

    Display *display = XtDisplay(widget);

    if (current_cursor == 0)
	current_cursor = XCreateFontCursor(display, XC_top_left_arrow);

    old_cursor = current_cursor;

    if (XtIsRealized(w))
    {
	XDefineCursor(display, XtWindow(widget), hourglass_cursor());
	XFlush(display);
    }
}

_Delay::~_Delay()
{
    if (widget == 0)
	return;

    if (XtIsRealized(widget))
    {
	XDefineCursor(XtDisplay(widget), XtWindow(widget), old_cursor);
	XFlush(XtDisplay(widget));
    }

    current_cursor = old_cursor;
}


int Delay::delay_count = 0;
WidgetArray Delay::_shells;
DelayArray Delay::delays;

Delay::Delay(Widget w):
    _Delay(w)
{
    if (delay_count++ == 0)
    {
	for (int i = 0; i < delays.size(); i++)
	{
	    assert(delays[i] == 0);
	    if (_shells[i])
		delays[i] = new _Delay(_shells[i]);
	}
    }
}

void Delay::DestroyCB(Widget w, XtPointer, XtPointer)
{
    // Unregister shell
    for (int i = 0; i < _shells.size(); i++)
	if (_shells[i] == w)
	{
	    _shells[i] = 0;
	    if (delays[i])
	    {
		delete delays[i];
		delays[i] = 0;
	    }
	}
}

void Delay::register_shell(Widget w)
{
    // Check if the shell is already registered
    for (int i = 0; i < _shells.size(); i++)
	if (_shells[i] == w)
	    return;

    // Look for an empty slot
    for (i = 0; i < _shells.size() && _shells[i] != 0; i++)
	;

    XtAddCallback(w, XtNdestroyCallback, DestroyCB, 0);

    _Delay *new_delay = 0;
    if (delay_count)
	new_delay = new _Delay(w);

    if (i >= _shells.size())
    {
	_shells += w;
	delays  += new_delay;
    }
    else
    {
	assert(_shells[i] == 0);
	assert(delays[i] == 0);

	_shells[i] = w;
	delays[i]  = new_delay;
    }
}

Delay::~Delay()
{
    if (--delay_count == 0)
    {
	for (int i = 0; i < delays.size(); i++)
	{
	    if (delays[i])
	    {
		delete delays[i];
		delays[i] = 0;
	    }
	}
    }
}
