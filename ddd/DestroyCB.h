// $Id$
// Destroy Callbacks

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

#ifndef _DDD_DestroyCB_h
#define _DDD_DestroyCB_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

#if defined(IF_XM)
// Destroy WIDGET as soon as we are idle
extern void DestroyWhenIdle(Widget widget);

// Destroy the surrounding shell
extern void DestroyShellCB(Widget widget, XtPointer client_data,
			   XtPointer call_data);

// Destroy specific widget
extern void DestroyThisCB(Widget widget, XtPointer client_data,
			  XtPointer call_data);

// Unmanage the surrounding shell
extern void UnmanageShellCB(Widget widget, XtPointer client_data,
			    XtPointer call_data);

// Unmanage specific widget
extern void UnmanageThisCB(Widget widget, XtPointer client_data,
			   XtPointer call_data);
#else
// Destroy WIDGET as soon as we are idle
extern void DestroyWhenIdle(GUI::Widget *widget);

// Destroy the surrounding shell
extern void DestroyShellCB(GUI::Widget *);

// Destroy specific widget
extern void DestroyThisCB(GUI::Widget *);

// Unmanage the surrounding shell
extern void UnmanageShellCB(GUI::Widget *);

// Unmanage specific widget
extern void UnmanageThisCB(GUI::Widget *);
#endif

#endif
