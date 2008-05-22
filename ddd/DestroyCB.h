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

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

#include "gtk_wrapper.h"

// Destroy WIDGET as soon as we are idle
#if defined(IF_XM)

extern void DestroyWhenIdle(Widget widget);

// Destroy the surrounding shell
extern void DestroyShellCB(Widget, XtPointer, XtPointer);

#else

extern void DestroyWhenIdle(GUI::Widget *widget);

// Destroy the surrounding shell
extern void DestroyShellCB(GUI::Widget *);

#endif

#if defined(IF_XM)

// Destroy specific widget
extern void DestroyThisCB(Widget, XtPointer, XtPointer);

// Unmanage the surrounding shell
extern void UnmanageShellCB(Widget, XtPointer, XtPointer);

#else

// Destroy specific widget
extern void DestroyThisCB(GUI::Widget *);

// Unmanage the surrounding shell
extern void UnmanageShellCB(GUI::Widget *);

#endif

// Unmanage specific widget
#if defined(IF_XM)

extern void UnmanageThisCB(Widget, XtPointer client_data, XtPointer);

#else

extern void UnmanageThisCB(GUI::Widget *);

#endif

#endif
