// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char Delay_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "DynArray.h"
#pragma implementation "VarArray.h"
#endif

#define LOG_DELAY 0

#include "Delay.h"
#include "assert.h"
#include "longName.h"
#include "HelpCB.h"
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

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

void (*Delay::shell_registered)(Widget w) = 0;

Cursor _Delay::hourglass_cache = 0;
Cursor _Delay::current_cursor = 0;

Cursor _Delay::hourglass_cursor()
{
    if (hourglass_cache != 0)
	return hourglass_cache;

    Display *display = XtDisplay(widget);

#if defined(SMALL_HOURGLASS_CURSOR) || defined(LARGE_HORGLASS_CURSOR)
    Screen *screen = XtScreen(widget);
    Window rootWindow = RootWindowOfScreen(screen);

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
    cursor_colors[0].pixel = BlackPixelOfScreen(screen);
    cursor_colors[1].pixel = WhitePixelOfScreen(screen);

    XQueryColors(display, DefaultColormapOfScreen(screen), 
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
    widget(w), old_cursor(0)
{
    if (widget == 0)
	return;

#if LOG_DELAY
    clog << "Setting " << XtName(widget) << " delay cursor\n";
#endif

    Display *display = XtDisplay(widget);

    if (current_cursor == 0)
    {
	// XC_left_ptr is the default cursor in OSF/Motif.
	// (How do we determine the current cursor? - FIXME)
	current_cursor = XCreateFontCursor(display, XC_left_ptr);
    }

    old_cursor = current_cursor;

    if (XtIsRealized(widget))
    {
	XDefineCursor(display, XtWindow(widget), hourglass_cursor());
	XFlush(display);
    }

    XtAddCallback(widget, XtNdestroyCallback, _Delay::DestroyCB, this);
}

_Delay::~_Delay()
{
    if (widget == 0 || old_cursor == 0)
	return;

#if LOG_DELAY
    clog << "Removing " << XtName(widget) << " delay cursor\n";
#endif

    if (XtIsRealized(widget))
    {
	XDefineCursor(XtDisplay(widget), XtWindow(widget), old_cursor);
	XFlush(XtDisplay(widget));
    }

    current_cursor = old_cursor;
    XtRemoveCallback(widget, XtNdestroyCallback, _Delay::DestroyCB, this);
}

// Make sure we do not attempt to delete a delay on a destroyed widget
void _Delay::DestroyCB(Widget, XtPointer client_data, XtPointer)
{
    _Delay *delay = (_Delay *)client_data;
    assert(ptr_cast(_Delay, delay));

    delay->widget     = 0;
    delay->old_cursor = 0;
}


int Delay::delay_count = 0;
WidgetArray Delay::_shells;
DelayArray Delay::delays;

Delay::Delay(Widget w):
    _Delay(w)
{
    assert(delays.size() == _shells.size());

    if (delay_count++ == 0)
    {
	for (int i = 0; i < delays.size(); i++)
	{
	    assert(delays[i] == 0);
	    if (_shells[i])
	    {
#if LOG_DELAY
		clog << "Slot " << i << ": ";
#endif
		delays[i] = new _Delay(_shells[i]);
	    }
	}
    }
}

// Make sure the shell is unregistered when destroyed
void Delay::DestroyCB(Widget widget, XtPointer, XtPointer)
{
    assert(delays.size() == _shells.size());

    // Unregister shell
    for (int i = 0; i < _shells.size(); i++)
	if (_shells[i] == widget)
	{
	    _shells[i] = 0;
	    if (delays[i])
	    {
		delete delays[i];
		delays[i] = 0;
	    }
#if LOG_DELAY
	    clog << "Unregistering " << XtName(widget) 
		 << " in slot " << i << "\n";
#endif
	}
}

void Delay::register_shell(Widget widget)
{
    assert(delays.size() == _shells.size());

    // Check if the shell is already registered
    int i;
    for (i = 0; i < _shells.size(); i++)
	if (_shells[i] == widget)
	    return;

    // Look for an empty slot
    for (i = 0; i < _shells.size() && _shells[i] != 0; i++)
	;

    XtAddCallback(widget, XtNdestroyCallback, DestroyCB, XtPointer(0));

    _Delay *new_delay = 0;
    if (delay_count)
	new_delay = new _Delay(widget);

    if (i == _shells.size())
    {
	_shells += Widget(0);
	delays  += (_Delay *)0;
    }

    assert(_shells[i] == 0);
    assert(delays[i] == 0);
    
    _shells[i] = widget;
    delays[i]  = new_delay;

#if LOG_DELAY
    clog << "Registering " << XtName(widget) << " in slot " << i << "\n";
#endif

    if (shell_registered != 0)
	shell_registered(widget);

    assert(delays.size() == _shells.size());
}

Delay::~Delay()
{
    assert(delays.size() == _shells.size());

    if (--delay_count == 0)
    {
	for (int i = 0; i < delays.size(); i++)
	{
	    if (delays[i])
	    {
#if LOG_DELAY
		clog << "Slot " << i << ": ";
#endif
		delete delays[i];
		delays[i] = 0;
	    }
	}
    }
}


// bullmann@fzi.de (Joerg Bullmann) says a dummy comment is required here...
