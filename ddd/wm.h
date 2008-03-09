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

#include <X11/Intrinsic.h>
#include "strclass.h"

// Window manager
extern void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask);
extern void wm_set_icon(Display *display, Window shell,
			Pixmap icon, Pixmap mask);

extern void wm_set_name(Widget shell, string title = "", string icon = "");
extern void wm_set_name(Display *display, Window shell,
			string title = "", string icon = "");

// Misc functions
void wait_until_mapped(Widget w, Widget shell = 0);
void raise_shell(Widget w);
void manage_and_raise(Widget w);

#endif // _DDD_wm_h
// DON'T ADD ANYTHING BEHIND THIS #endif
