// $Id$ -*- C++ -*-
// Modify debugger settings

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_settings_h
#define _DDD_settings_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"
#include "strclass.h"
#include "GDBAgent.h"

// Debugger settings

// Popup editor for debugger settings
extern void dddPopupSettingsCB(Widget, XtPointer, XtPointer);

// Process `show' output
extern void process_show(string command, string value, bool init = false);

// True if settings might have changed
extern bool need_settings();

// Get settings string
extern string get_settings(DebuggerType type);

// Get help on `DBXENV BASE'
extern string get_dbx_help(string dbxenv, string base);
		    
// Mark settings as `saved'
extern void save_settings_state();

// Mark settings as `invalid'
extern void reset_settings();


// User Status Displays

// Popup editor for debugger infos
extern void dddPopupInfosCB(Widget, XtPointer, XtPointer);

// Update debugger infos
extern void update_infos();

// Register additional info button
extern void register_info_button(Widget w);


// Command Definitions

// True if definitions might have changed
extern bool need_defines();

// Call this function if command definitions have changed
extern void set_need_defines(bool val = true);

// Get current definitions
extern string get_defines(DebuggerType type);

// Define command
extern void dddDefineCommandCB(Widget, XtPointer, XtPointer);

// Update define buttons
void UpdateDefinePanelCB(Widget = 0, XtPointer = 0, XtPointer = 0);

#endif // _DDD_settings_h
// DON'T ADD ANYTHING BEHIND THIS #endif
