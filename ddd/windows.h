// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_windows_h
#define _DDD_windows_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>

// Shells (only used if separate windows are used)
extern Widget command_shell;
extern Widget data_disp_shell;
extern Widget source_view_shell;

// Shell management
extern void initial_popup_shell(Widget w);
extern void popdown_shell(Widget w);
extern int running_shells();

extern void gdbOpenCommandWindowCB  (Widget, XtPointer, XtPointer);
extern void gdbOpenSourceWindowCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenDataWindowCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenExecWindowCB     (Widget, XtPointer, XtPointer);

extern void gdbCloseCommandWindowCB (Widget, XtPointer, XtPointer);
extern void gdbCloseSourceWindowCB  (Widget, XtPointer, XtPointer);
extern void gdbCloseDataWindowCB    (Widget, XtPointer, XtPointer);
extern void gdbCloseExecWindowCB    (Widget, XtPointer, XtPointer);

// Register this event handler with all shells
extern void StructureNotifyEH(Widget, XtPointer, XEvent *, Boolean *);

#endif // _DDD_windows_h
// DON'T ADD ANYTHING BEHIND THIS #endif
