// $Id$
// Destroy Callbacks

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

#ifndef _DDD_DestroyCB_h
#define _DDD_DestroyCB_h

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

#include "gtk_wrapper.h"

// Destroy WIDGET as soon as we are idle
#if defined(IF_XM)
extern void DestroyWhenIdle(Widget widget);
#elif defined(IF_XMMM)
extern void DestroyWhenIdle(Widget widget);
extern void DestroyWhenIdle1(GUI::Widget *widget);
#else
extern void DestroyWhenIdle(Gtk::Widget *widget);
extern void DestroyWhenIdle1(GUI::Widget *widget);
#endif

// Destroy the surrounding shell
extern void DestroyShellCB(CB_ARG_LIST_1());

// Destroy specific widget
#ifdef IF_MOTIF
extern void DestroyThisCB(Widget, XtPointer client_data, XtPointer);
#else // NOT IF_MOTIF
extern void DestroyThisCB(Widget);
#endif // IF_MOTIF

// Unmanage the surrounding shell
extern void UnmanageShellCB(CB_ARG_LIST_1());

// Unmanage specific widget
#if defined(IF_MOTIF)
extern void UnmanageThisCB1(Widget, XtPointer client_data, XtPointer); // FIXME compat
#endif

#if !defined(IF_XM)
extern void UnmanageThisCB(GUI::Widget *);
extern void UnmanageThisCB2(Widget); // FIXME compat
#endif

#endif
