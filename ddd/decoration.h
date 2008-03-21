// $Id$ -*- C++ -*-
// Check for WM decoration

// Copyright (C) 1999 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_decoration_h
#define _DDD_decoration_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_MOTIF)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

#include "gtk_wrapper.h"

#include "bool.h"

#if defined(IF_XM)
// Return a transient position on SCREEN (for command tool etc.) in POS_X/POS_Y
extern void get_transient_pos(Screen *screen,
			      Position& pos_x, Position& pos_y);
#else
// Return a transient position on SCREEN (for command tool etc.) in POS_X/POS_Y
extern void get_transient_pos(GUI::RefPtr<GUI::Screen> screen,
			      Position& pos_x, Position& pos_y);
#endif

// Start check for window manager decoration
extern void start_have_decorated_transients(Widget parent);

// Return true if transient windows are decorated
extern bool have_decorated_transients(Widget parent);

#endif // _DDD_decoration_h
// DON'T ADD ANYTHING BEHIND THIS #endif
