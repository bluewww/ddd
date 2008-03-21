// $Id$ -*- C++ -*-
// DDD logos and logo functions

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_logo_h
#define _DDD_logo_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_MOTIF)
#include <X11/Intrinsic.h>
#else
#endif

#if !defined(IF_XM)
#include <GUI/Image.h>
#endif

#include "strclass.h"
#include "version.h"
#include "MString.h"

// DDD logos

#if defined(IF_XM)
// Return a pixmap suitable for icons on the root window
extern Pixmap iconlogo(Widget shell);
extern Pixmap iconmask(Widget shell);

// Return the DDD splash screen.  COLOR_KEY indicates the XPM visual type.
extern Pixmap dddsplash(Widget shell, 
			const string& color_key,
			Dimension& width, Dimension& height);
#else
// Return a pixmap suitable for icons on the root window
extern GUI::RefPtr<GUI::Pixmap> iconlogo(GUI::Widget *shell);
extern GUI::RefPtr<GUI::Pixmap> iconmask(GUI::Widget *shell);

// Return the DDD splash screen.  COLOR_KEY indicates the XPM visual type.
extern GUI::RefPtr<GUI::Pixmap> dddsplash(GUI::Widget *shell, 
					  const string& color_key,
					  Dimension& width, Dimension& height);
#endif

#if defined(IF_XM)
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
#else
// Install toolbar icons in Motif cache.  COLOR_KEY indicates the XPM
// visual type for inactive buttons.  ACTIVE_COLOR_KEY is the XPM visual
// type for active buttons (entered or armed).
extern void install_icons(GUI::Widget *shell, 
			  const string& color_key,
			  const string& active_color_key);
inline void install_icons(GUI::Widget *shell, 
			  const string& color_key = 'c')
{
    install_icons(shell, color_key, color_key);
}
#endif

// Set label of W to NEW_LABEL (and its pixmap to IMAGE_NAME, if given)
#if defined(IF_XM)
extern void set_label(Widget w, const MString& new_label, 
		      const char *image_name = 0);
#else
extern void set_label(Widget w, const MString& new_label, 
		      GUI::ImageHandle *image_name = 0);
#endif

#if !defined(IF_MOTIF)
extern MString get_label(Widget w);
#endif
#if !defined(IF_XM)
extern MString get_label(GUI::Widget *w);
#endif

// Icon names
#if defined(IF_XM)
#define DDD_ICON           ddd_NAME
#define BREAK_AT_ICON      "break_at"
#define CLEAR_AT_ICON      "clear_at"
#define CLUSTER_ICON       "cluster"
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
#define UNCLUSTER_ICON     "uncluster"
#define UNDISPLAY_ICON     "undisplay"
#define UNWATCH_ICON       "unwatch"
#define WATCH_ICON         "watch"
#else
extern GUI::ImageHandle DDD_ICON[1];
extern GUI::ImageHandle BREAK_AT_ICON[4];
extern GUI::ImageHandle CLEAR_AT_ICON[4];
extern GUI::ImageHandle CLUSTER_ICON[4];
extern GUI::ImageHandle DELETE_ICON[4];
extern GUI::ImageHandle DISABLE_ICON[4];
extern GUI::ImageHandle DISPREF_ICON[4];
extern GUI::ImageHandle DISPLAY_ICON[4];
extern GUI::ImageHandle ENABLE_ICON[4];
extern GUI::ImageHandle FIND_BACKWARD_ICON[4];
extern GUI::ImageHandle FIND_FORWARD_ICON[4];
extern GUI::ImageHandle HIDE_ICON[4];
extern GUI::ImageHandle LOOKUP_ICON[4];
extern GUI::ImageHandle MAKETEMP_ICON[4];
extern GUI::ImageHandle NEW_BREAK_ICON[4];
extern GUI::ImageHandle NEW_DISPLAY_ICON[4];
extern GUI::ImageHandle NEW_WATCH_ICON[4];
extern GUI::ImageHandle PLOT_ICON[4];
extern GUI::ImageHandle PRINT_ICON[4];
extern GUI::ImageHandle PROPERTIES_ICON[4];
extern GUI::ImageHandle ROTATE_ICON[4];
extern GUI::ImageHandle SET_ICON[4];
extern GUI::ImageHandle SHOW_ICON[4];
extern GUI::ImageHandle UNCLUSTER_ICON[4];
extern GUI::ImageHandle UNDISPLAY_ICON[4];
extern GUI::ImageHandle UNWATCH_ICON[4];
extern GUI::ImageHandle WATCH_ICON[4];
#endif

#endif // _DDD_logo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
