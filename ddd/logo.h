// $Id$ -*- C++ -*-
// DDD logos and logo functions

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

// Return the DDD splash screen.  COLOR_KEY indicates the XPM visual type.
extern Pixmap dddsplash(Widget shell, const string& color_key = 'c');

// Install toolbar icons in Motif cache.  COLOR_KEY indicates the XPM
// visual type for inactive buttons.  ACTIVE_COLOR_KEY is the XPM visual
// type for active buttons (entered or armed).
extern void install_icons(Widget shell, 
			  const string& color_key,
			  const string& active_color_key);
inline void install_icons(Widget shell, 
			  const string& color_key = 'c')
{
    install_icons(shell, color_key, color_key);
}

// Set label of W to NEW_LABEL (and its pixmap to IMAGE_NAME, if given)
extern void set_label(Widget w, const MString& new_label, 
		      char *image_name = 0);

// Icon names
#define DDD_ICON           ddd_NAME
#define BREAK_AT_ICON      "break_at"
#define CLEAR_AT_ICON      "clear_at"
#define DELETE_ICON        "delete"
#define DISABLE_ICON       "disable"
#define DISPREF_ICON       "dispref"
#define DISPLAY_ICON       "display"
#define ENABLE_ICON        "enable"
#define FIND_BACKWARD_ICON "find_backward"
#define FIND_FORWARD_ICON  "find_forward"
#define HIDE_ICON          "hide"
#define LOOKUP_ICON        "lookup"
#define MAKETEMP_ICON      "maketemp"
#define NEW_BREAK_ICON     "new_break"
#define NEW_DISPLAY_ICON   "new_display"
#define NEW_WATCH_ICON     "new_watch"
#define PLOT_ICON          "plot"
#define PRINT_ICON         "print"
#define PROPERTIES_ICON    "properties"
#define ROTATE_ICON        "rotate"
#define SET_ICON           "set"
#define SHOW_ICON          "show"
#define UNDISPLAY_ICON     "undisplay"
#define UNWATCH_ICON       "unwatch"
#define WATCH_ICON         "watch"

#endif // _DDD_logo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
