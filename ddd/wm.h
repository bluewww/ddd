// $Id$ -*- C++ -*-
// DDD window manager functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_wm_h
#define _DDD_wm_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_MOTIF)
#include <X11/Intrinsic.h>
#endif

#if !defined(IF_XM)
#include <GUI/Widget.h>
#endif

#include "gtk_wrapper.h"

#include "strclass.h"

#if defined(IF_XM)
extern void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask);
extern void wm_set_icon(Display *display, Window shell,
			Pixmap icon, Pixmap mask);
#else
// Window manager
extern void wm_set_icon(GUI::Widget *shell, GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask);
extern void wm_set_icon(GUI::RefPtr<GUI::Display> display, GUI::RefPtr<GUI::XWindow> shell,
			GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask);
#endif

extern void wm_set_name(Widget shell, string title = "", string icon = "");
#if defined(IF_XM)
extern void wm_set_name(Display *display, Window shell,
			string title = "", string icon = "");
#else
extern void wm_set_name(GUI::RefPtr<GUI::Display> display, GUI::RefPtr<GUI::XWindow> shell,
			string title = "", string icon = "");
#endif

// Misc functions
#if defined(IF_XM)
void wait_until_mapped(Widget w, Widget shell = 0);
void raise_shell(Widget w);
#else
void wait_until_mapped(GUI::Widget *w, GUI::Widget *shell = 0);
void raise_shell(GUI::Widget *w);
#endif

#if defined(IF_XM)
void manage_and_raise(Widget w);
#elif defined(IF_XMMM)
void manage_and_raise(Widget w);
void manage_and_raise1(GUI::Widget *w);
#else
void manage_and_raise(Gtk::Widget *w);
void manage_and_raise1(GUI::Widget *w);
#endif

#endif // _DDD_wm_h
// DON'T ADD ANYTHING BEHIND THIS #endif
