// $Id$ -*- C++ -*-
// Modify debugger settings

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_settings_h
#define _DDD_settings_h

#include <X11/Intrinsic.h>
#include "bool.h"
#include "strclass.h"
#include "GDBAgent.h"
#include "StringA.h"
#include "options.h"

// Debugger settings

// Popup editor for debugger settings
extern void dddPopupSettingsCB(Widget, XtPointer, XtPointer);

// Process `show' output
extern void process_show(const string& command, string value, bool init = false);

// True if settings might have changed
extern bool need_settings();

// Update settings tables
extern void update_settings();

// Get settings string
extern string get_settings(DebuggerType type, 
			   unsigned long flags = SAVE_DEFAULT);

// Get help on `DBXENV BASE'
extern string get_dbx_help(const string& dbxenv, const string& base);

// Mark settings as `saved'
extern void save_settings_state();

// Mark settings as `invalid'
extern void reset_settings();

// Get `show' command for settings command CMD
extern string show_command(const string& cmd, DebuggerType type);


// User Status Displays

// Popup editor for debugger infos
extern void dddPopupInfosCB(Widget, XtPointer, XtPointer);

// Update debugger infos
extern void update_infos();

// Register additional info button
extern void register_info_button(Widget w);


// Signal Handling

// Popup editor for signal handling
extern void dddPopupSignalsCB(Widget, XtPointer, XtPointer);

// Process `handle' output
extern void process_handle(string line, bool init = false);

// True if signals might have changed
extern bool need_signals();

// Update signal tables
extern void update_signals();

// Get signals string
extern string get_signals(DebuggerType type,
			  unsigned long flags = SAVE_DEFAULT);

// Mark signals as `saved'
extern void save_signals_state();

// Mark signals as `invalid'
extern void reset_signals();



// Theme Handling

// Return all available themes
void get_themes(StringArray& themes);

// Update theme table
extern void update_themes();

// Popup themes editor
extern void dddPopupThemesCB(Widget, XtPointer, XtPointer);


// Command Definitions

// True if definitions must be reloaded
extern bool need_load_defines();

// Call this function if command definitions have changed
extern void set_need_load_defines(bool val = true);
extern void set_need_save_defines(bool val = true);

// True if definitions must be saved
extern bool need_save_defines();

// Get current definitions
extern string get_defines(DebuggerType type,
			  unsigned long flags = SAVE_DEFAULT);

// True if defined command
extern bool is_defined_cmd(const string& command);

// Update definition of COMMAND at next GDB prompt
void update_define_later(const string& command);

// Define command
extern void dddDefineCommandCB(Widget, XtPointer, XtPointer);

// Update define buttons
void UpdateDefinePanelCB(Widget = 0, XtPointer = 0, XtPointer = 0);

#endif // _DDD_settings_h
// DON'T ADD ANYTHING BEHIND THIS #endif
