// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_logo_h
#define _DDD_logo_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "strclass.h"
#include "version.h"
#include "MString.h"

// DDD logos

// Return a pixmap suitable for icons on the root window
extern Pixmap iconlogo(Widget shell);
extern Pixmap iconmask(Widget shell);

// Return a small DDD logo suitable for the widget W
extern Pixmap versionlogo(Widget shell);

// Return the DDD splash screen
extern Pixmap dddsplash(Widget shell, const string& color_key = 'c');

// Install toolbar icons in Motif cache
extern void install_icons(Widget shell, const string& color_key = 'c');

// Set label of W to NEW_LABEL (and its pixmap to IMAGE_NAME, if given)
extern void set_label(Widget w, const MString& new_label, 
		      char *image_name = 0);

// Set sensitivity of W to STATE
inline void set_sensitive(Widget w, bool state)
{
    if (w != 0)
	XtSetSensitive(w, state);
}

// Manage W iff STATE
inline void manage_child(Widget w, bool state)
{
    if (w != 0)
    {
	if (state)
	    XtManageChild(w);
	else
	    XtUnmanageChild(w);
    }
}

// Icon names
#define DDD_ICON           ddd_NAME
#define BREAK_AT_ICON      "break_at"
#define CLEAR_AT_ICON      "clear_at"
#define DISPREF_ICON       "dispref"
#define DISPLAY_ICON       "display"
#define FIND_FORWARD_ICON  "find_forward"
#define FIND_BACKWARD_ICON "find_backward"
#define HIDE_ICON          "hide"
#define LOOKUP_ICON        "lookup"
#define PRINT_ICON         "print"
#define ROTATE_ICON        "rotate"
#define SET_ICON           "set"
#define SHOW_ICON          "show"
#define UNDISPLAY_ICON     "undisplay"
#define UNWATCH_ICON       "unwatch"
#define WATCH_ICON         "watch"

#endif // _DDD_logo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
