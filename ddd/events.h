// $Id$
// Event access

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_events_h
#define _DDD_events_h

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef IF_MOTIF

#include <X11/Xlib.h>

#else // NOT IF_MOTIF

#include "gtk_wrapper.h"

#endif // IF_MOTIF

#if !defined(IF_XM)
#include <GUI/Events.h>
#endif

#include "BoxPoint.h"
#include "BoxSize.h"

#if defined(IF_MOTIF)
BoxPoint point(XEvent *ev); // Location
BoxSize size(XEvent *ev);   // Size
Time time(XEvent *ev);      // Time
#endif

#if !defined(IF_XM)
BoxPoint point1(GUI::Event *ev); // Location
BoxSize size1(GUI::Event *ev);   // Size
Time time1(GUI::Event *ev);      // Time
#endif

#endif
