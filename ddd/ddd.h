// $Id$
// DDD application resources

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_ddd_h
#define _DDD_ddd_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include <fstream.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "bool.h"

// GDB command widget
extern Widget gdb_w;

// Status line widget
extern Widget status_w;

// Output TEXT in GDB window, unless we've just composed a command
extern void gdb_out(const string& text);

// Output TEXT in GDB window (unconditionally)
extern void _gdb_out(const string& text);

// Update option menus
extern void update_options();

// Update `Reset' button in preferences panel
extern void update_reset_preferences();

// Update argument buttons
extern void update_arg_buttons();

// Update user-defined buttons
extern void update_user_buttons();

// Save current option state
extern void save_option_state();

// Last output position
extern XmTextPosition promptPosition;

// Last message position
extern XmTextPosition messagePosition;

// True if output is running
extern bool private_gdb_output;

// True if input is running
extern bool private_gdb_input;

// True if GDB issued initial prompt
extern bool gdb_initialized;

// Some more global objects
extern class GDBAgent   *gdb;
extern class DataDisp   *data_disp;
extern class SourceView *source_view;
extern class ArgField   *source_arg;

// Where to log DDD I/O
extern ostream *_dddlog;
#define dddlog (*_dddlog)

// Text to ignore upon output
extern string gdb_out_ignore;

// Process next DDD event
extern void process_next_event();

// Process all pending DDD events
extern void process_pending_events();

// Setup is done
extern Boolean ddd_setup_done(XtPointer client_data);

// Register shells of menu ITEMS.
extern void register_menu_shell(struct MMDesc items[]);

// Process ^C key
extern bool process_emergencies();

#endif // _DDD_ddd_h
// DON'T ADD ANYTHING BEHIND THIS #endif
