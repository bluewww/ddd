// $Id$ -*- C++ -*-
// DDD window management

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_windows_h
#define _DDD_windows_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"

// Shells (only used if separate windows are used)
extern Widget command_shell;
extern Widget data_disp_shell;
extern Widget source_view_shell;

// Command tool
extern Widget tool_shell;
extern Widget tool_buttons_w;

// Shell management
extern void initial_popup_shell(Widget w);
extern void popup_shell(Widget w);
extern void popdown_shell(Widget w);
extern void iconify_shell(Widget w);
extern void uniconify_shell(Widget w);
extern int running_shells();

// True if W was started iconified
extern bool started_iconified(Widget w);

extern void gdbOpenCommandWindowCB  (Widget, XtPointer, XtPointer);
extern void gdbOpenSourceWindowCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenCodeWindowCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenDataWindowCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenExecWindowCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenToolWindowCB     (Widget, XtPointer, XtPointer);

extern void gdbCloseCommandWindowCB (Widget, XtPointer, XtPointer);
extern void gdbCloseSourceWindowCB  (Widget, XtPointer, XtPointer);
extern void gdbCloseCodeWindowCB    (Widget, XtPointer, XtPointer);
extern void gdbCloseDataWindowCB    (Widget, XtPointer, XtPointer);
extern void gdbCloseExecWindowCB    (Widget, XtPointer, XtPointer);
extern void gdbCloseToolWindowCB    (Widget, XtPointer, XtPointer);

extern void gdbToggleCommandWindowCB(Widget, XtPointer, XtPointer);
extern void gdbToggleSourceWindowCB (Widget, XtPointer, XtPointer);
extern void gdbToggleCodeWindowCB   (Widget, XtPointer, XtPointer);
extern void gdbToggleDataWindowCB   (Widget, XtPointer, XtPointer);
extern void gdbToggleExecWindowCB   (Widget, XtPointer, XtPointer);
extern void gdbToggleToolWindowCB   (Widget, XtPointer, XtPointer);

extern bool have_command_window();
extern bool have_source_window();
extern bool have_code_window();
extern bool have_data_window();
extern bool have_exec_window();
extern bool have_tool_window();


// Close current window
extern void DDDCloseCB              (Widget, XtPointer, XtPointer);

// Register this event handler with all shells
extern void StructureNotifyEH(Widget, XtPointer, XEvent *, Boolean *);

// Save current tool shell offset in APP_DATA.
extern void get_tool_offset();

// Manage child with minimum size
extern void manage_paned_child(Widget w);
extern void unmanage_paned_child(Widget w);

// Promote size of ScrolledWindow child CHILD to TARGET (default: parent)
extern void set_scrolled_window_size(Widget child, Widget target = 0);

// Set the width of PANED to the maximum width of its children
extern void get_paned_window_width(Widget paned, Dimension& max_width);
extern void set_paned_window_size(Widget paned, Dimension width);

// Set main window sizes
extern void set_main_window_size(Widget w);

// Save paned child sizes
extern void save_preferred_paned_sizes(Widget paned);

#endif // _DDD_windows_h
// DON'T ADD ANYTHING BEHIND THIS #endif
