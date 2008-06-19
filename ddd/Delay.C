// $Id$  -*- C++ -*-
// Delay class -- displays an hourglass cursor until destroyed

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char Delay_rcsid[] = 
    "$Id$";

#define LOG_DELAY 1

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "Delay.h"
#include "assert.h"
#include "longName.h"
#include "HelpCB.h"
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#if defined(IF_XM)
// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#else
#include <GUI/Cursor.h>
#endif

DEFINE_TYPE_INFO_0(_Delay);
DEFINE_TYPE_INFO_0(Delay);

#define SMALL_HOURGLASS_CURSOR 1

#if SMALL_HOURGLASS_CURSOR || LARGE_HOURGLASS_CURSOR
#include "icons/cursors/time16.xbm"
#include "icons/cursors/time16m.xbm"
#if time16_width != time16m_width || time16_height != time16m_height
#error Bad mask for time16
#endif
#endif

#if LARGE_HOURGLASS_CURSOR
#include "icons/cursors/time32.xbm"
#include "icons/cursors/time32m.xbm"
#if time32_width != time32m_width || time32_height != time32m_height
#error Bad mask for time32
#endif
#endif

#if defined(IF_XM)
void (*Delay::shell_registered)(Widget w) = 0;

Cursor _Delay::hourglass_cache = 0;
Cursor _Delay::current_cursor = 0;
#else
void (*Delay::shell_registered)(GUI::Widget *w) = 0;

GUI::Cursor *_Delay::hourglass_cache = 0;
GUI::Cursor *_Delay::current_cursor = 0;
#endif

#if defined(IF_XM)
Cursor _Delay::hourglass_cursor()
{
    if (hourglass_cache != 0)
	return hourglass_cache;

    Display *display = XtDisplay(widget);

#if SMALL_HOURGLASS_CURSOR || LARGE_HORGLASS_CURSOR
    Screen *screen = XtScreen(widget);
    Window rootWindow = RootWindowOfScreen(screen);

    unsigned char *cursor_bits      = time16_bits;
    unsigned char *cursor_mask_bits = time16m_bits;
    unsigned int cursor_width       = time16_width;
    unsigned int cursor_height      = time16_height;
    unsigned int cursor_x_hot       = time16_x_hot;
    unsigned int cursor_y_hot       = time16_y_hot;

#if LARGE_HOURGLASS_CURSOR
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
	XCreateBitmapFromData(display, rootWindow, (char *)cursor_bits,
			      cursor_width, cursor_height);
    Pixmap cursor_mask_pixmap = 
	XCreateBitmapFromData(display, rootWindow, (char *)cursor_mask_bits,
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
#else
GUI::Cursor *_Delay::hourglass_cursor()
{
    if (hourglass_cache != 0)
	return hourglass_cache;

    GUI::RefPtr<GUI::Display> display = widget->get_display();

#if SMALL_HOURGLASS_CURSOR || LARGE_HORGLASS_CURSOR
    GUI::RefPtr<GUI::Screen> screen = widget->get_screen();
    GUI::RefPtr<GUI::XWindow> rootWindow = screen->get_root_window();

    unsigned char *cursor_bits      = time16_bits;
    unsigned char *cursor_mask_bits = time16m_bits;
    unsigned int cursor_width       = time16_width;
    unsigned int cursor_height      = time16_height;
    unsigned int cursor_x_hot       = time16_x_hot;
    unsigned int cursor_y_hot       = time16_y_hot;

#if LARGE_HOURGLASS_CURSOR
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

    GUI::RefPtr<GUI::Pixmap> cursor_pixmap = 
	GUI::Pixmap::create_bitmap_from_data((char *)cursor_bits, cursor_width, cursor_height);
    GUI::RefPtr<GUI::Pixmap> cursor_mask_pixmap = 
	GUI::Pixmap::create_bitmap_from_data((char *)cursor_mask_bits, cursor_width, cursor_height);
#ifdef NAG_ME
#warning BlackPixelOfScreen, etc.?
#endif
    GUI::Color bg(0.0, 0.0, 0.0);
    GUI::Color fg(1.0, 1.0, 1.0);
    hourglass_cache = 
	new GUI::Cursor(cursor_pixmap, cursor_mask_pixmap, fg, bg,
			cursor_x_hot, cursor_y_hot);
    
#else // Watch cursor
    std::cerr << "Watch cursor: unimplemented.\n";
#endif

    return hourglass_cache;
}
#endif

#if defined(IF_XM)
_Delay::_Delay(Widget w):
    widget(w), old_cursor(0)
{
    if (widget == 0)
	return;

#if LOG_DELAY
    std::clog << "Setting " << XtName(widget) << " delay cursor\n";
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
#else
_Delay::_Delay(GUI::Widget *w):
    widget(w), old_cursor(0)
{
    if (widget == 0)
	return;

#if LOG_DELAY
    std::clog << "Setting " << widget->get_name().c_str() << " delay cursor\n";
#endif

    GUI::RefPtr<GUI::Display> display = widget->get_display();

    static int errcnt = 0;
    if (!errcnt++) std::cerr << "_Delay::_Delay: not implemented\n";

    if (current_cursor == 0)
    {
	// XC_left_ptr is the default cursor in OSF/Motif.
	// (How do we determine the current cursor? - FIXME)
	current_cursor = new GUI::Cursor(GUI::LEFT_PTR);
    }

    old_cursor = current_cursor;

    if (widget->is_realized())
    {
	widget->get_window()->set_cursor(*hourglass_cursor());
	display->flush();
    }

    widget->add_destroy_notify_callback(this, _Delay::DestroyCB);
}
#endif

#if defined(IF_XM)
_Delay::~_Delay()
{
  // TODO: race condition here
  // Need to flush the events, put a lock, do the test below, delete
  // the call back and release the lock.
    if (widget == 0 || old_cursor == 0)
	return;

#if LOG_DELAY
    std::clog << "Removing " << XtName(widget) << " delay cursor\n";
#endif

    if (XtIsRealized(widget))
    {
	XDefineCursor(XtDisplay(widget), XtWindow(widget), old_cursor);
	XFlush(XtDisplay(widget));
    }

    current_cursor = old_cursor;
    XtRemoveCallback(widget, XtNdestroyCallback, _Delay::DestroyCB, this);
}
#else
_Delay::~_Delay()
{
  // TODO: race condition here
  // Need to flush the events, put a lock, do the test below, delete
  // the call back and release the lock.
    if (widget == 0 || old_cursor == 0)
	return;

#if LOG_DELAY
    std::clog << "Removing " << widget->get_name().c_str() << " delay cursor\n";
#endif

    static int errcnt = 0;
    if (!errcnt++) std::cerr << "_Delay::_Delay: not implemented\n";

    if (widget->is_realized())
    {
	widget->get_window()->set_cursor(*old_cursor);
	widget->get_display()->flush();
    }

    current_cursor = old_cursor;

    widget->remove_destroy_notify_callback(this);
}
#endif

// Make sure we do not attempt to delete a delay on a destroyed widget
#if defined(IF_XM)
void _Delay::DestroyCB(Widget, XtPointer client_data, XtPointer)
#else
void *_Delay::DestroyCB(void *client_data)
#endif
{
    _Delay *delay = (_Delay *)client_data;
    assert(ptr_cast(_Delay, delay));

    delay->widget     = 0;
    delay->old_cursor = 0;
#if !defined(IF_XM)
    std::cerr << "Delay widget destroyed.  What is return value of DestroyCB?\n";
    return NULL;
#endif
}


int Delay::delay_count = 0;
WidgetArray Delay::_shells;
DelayArray Delay::delays;

#if defined(IF_XM)
Delay::Delay(Widget w):
#else
Delay::Delay(GUI::Widget *w):
#endif
    _Delay(w)
{
    // defined(IF_XM)
    assert(delays.size() == _shells.size());

    if (delay_count++ == 0)
    {
	for (int i = 0; i < delays.size(); i++)
	{
	    assert(delays[i] == 0);
	    if (_shells[i])
	    {
#if LOG_DELAY
		std::clog << "Slot " << i << ": ";
#endif
		delays[i] = new _Delay(_shells[i]);
	    }
	}
    }
}

// Make sure the shell is unregistered when destroyed
#if defined(IF_XM)
void Delay::DestroyCB(Widget widget, XtPointer, XtPointer)
#else
void Delay::DestroyCB(GUI::Widget *widget)
#endif
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
#if defined(IF_XM)
	    std::clog << "Unregistering " << XtName(widget) 
		      << " in slot " << i << "\n";
#else
	    std::clog << "Unregistering " << widget->get_name().c_str()
		      << " in slot " << i << "\n";
#endif
#endif
	}
}

#if defined(IF_XM)
void Delay::register_shell(Widget widget)
#else
void Delay::register_shell(GUI::Widget *widget)
#endif
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

#if defined(IF_XM)
    XtAddCallback(widget, XtNdestroyCallback, DestroyCB, XtPointer(0));
#else
#ifdef NAG_ME
#warning No signal_destroy()?
#endif
    // widget->signal_destroy().connect(sigc::bind(PTR_FUN(Delay::DestroyCB), widget));
#endif

    _Delay *new_delay = 0;
    if (delay_count)
	new_delay = new _Delay(widget);

    if (i == _shells.size())
    {
#if defined(IF_XM)
	_shells += Widget(0);
#else
	_shells += (GUI::Widget *)0;
#endif
	delays  += (_Delay *)0;
    }

    assert(_shells[i] == 0);
    assert(delays[i] == 0);
    
    _shells[i] = widget;
    delays[i]  = new_delay;

#if LOG_DELAY
#if defined(IF_XM)
    std::clog << "Registering " << XtName(widget) << " in slot " << i << "\n";
#else
    std::clog << "Registering " << widget->get_name().c_str() << " in slot " << i << "\n";
#endif
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
		std::clog << "Slot " << i << ": ";
#endif
		delete delays[i];
		delays[i] = 0;
	    }
	}
    }
}


// bullmann@fzi.de (Joerg Bullmann) says a dummy comment is required here...
