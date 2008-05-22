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

#ifndef _DDD_wm_h
#define _DDD_wm_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#include "gtk_wrapper.h"
#endif

#include "strclass.h"

#if defined(IF_XM)
extern void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask);
extern void wm_set_icon(Display *display, Window shell,
			Pixmap icon, Pixmap mask);

extern void wm_set_name(Widget shell, string title = "", string icon = "");
extern void wm_set_name(Display *display, Window shell,
			string title = "", string icon = "");
#else
// Window manager
extern void wm_set_icon(GUI::Widget *shell, GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask);
extern void wm_set_icon(GUI::RefPtr<GUI::Display> display, GUI::RefPtr<GUI::XWindow> shell,
			GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask);

extern void wm_set_name(GUI::Widget *shell, string title = "", string icon = "");
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
#else
void manage_and_raise(GUI::Widget *w);
#endif

#if !defined(IF_XM)
// Copy text from any widget to default clipboard.
// Glib::RefPtr<Gtk::Clipboard> clip =
//   Gtk::Clipboard::get_for_display(XtDisplay(w));
bool text_copy_from(GUI::Widget *w);
bool text_cut_from(GUI::Widget *w);
#endif

#endif // _DDD_wm_h
// DON'T ADD ANYTHING BEHIND THIS #endif
