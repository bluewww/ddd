// $Id$  -*- C++ -*-
// Nora Help Callbacks

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _Nora_HelpCB_h
#define _Nora_HelpCB_h

#include <X11/Intrinsic.h>
#include "MString.h"

// Select widget and call help on context.
// May be used in a menu entry "Help On Context".
extern void HelpOnContextCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help on current shell window.
// May be used in a menu entry "Help On Window".
extern void HelpOnWindowCB(Widget widget, XtPointer client_data, 
			   XtPointer call_data);

// Call help on the top-level window.
// May be used in a menu entry "Help On Version".
extern void HelpOnVersionCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help on help.
// May be used in a menu entry "Help On Help".
extern void HelpOnHelpCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help for associated widget.
// May be used as help callback for any primitive widget.
extern void ImmediateHelpCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help with "XmString s = (XmString)client_data" as text.
// May be used for unchanged text display.
extern void MStringHelpCB(Widget widget, XtPointer client_data, 
			  XtPointer call_data);

// Call help with "String s = (String)client_data" as text.
// May be used for unchanged text display.
extern void StringHelpCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help with "String filename = (String)client_data" as text
// May be used for file display.
extern void FileHelpCB(Widget widget, XtPointer client_data, 
		       XtPointer call_data);

// Call help with "String command = (String)client_data" as text.
// May be used for command output display.
extern void CommandHelpCB(Widget widget, XtPointer client_data, 
			  XtPointer call_data);

// Call help with the manual page of the program.
extern void HelpManualCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help with a built-in formatted manual page "String s =
// (String)client_data".
extern void ManualStringHelpCB(Widget widget, XtPointer client_data, 
			       XtPointer call_data);

// Create a help text if the contextHelpString resource is not found
extern MString (*DefaultHelpText)(Widget widget);

// Pixmap to display at ``help on version''
extern Pixmap (*helpOnVersionPixmapProc)(Widget w);

#endif // _Nora_HelpCB_h
// DON'T ADD ANYTHING BEHIND THIS #endif
