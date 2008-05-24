// $Id$
// DDD application resources

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_ddd_h
#define _DDD_ddd_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "strclass.h"
#include <fstream>

#include "gtk_wrapper.h"

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#else
#include <GUI/ScrolledText.h>
#include <GUI/Button.h>
#endif

#include "bool.h"

#if defined(IF_XM)
// Main window widget
extern Widget main_window;

// GDB command widget
extern Widget gdb_w;
#else
// Main window widget
extern GUI::Container *main_window;

// GDB command widget
extern GUI::ScrolledText *gdb_w;
#endif

#if defined(IF_XM)
// Status line widget
extern Widget status_w;
#else
// Status line widget
extern GUI::Button *status_w;
#endif

// Output TEXT in GDB window, unless we've just composed a command
extern void gdb_out(const string& text);

// Output TEXT in GDB window (unconditionally)
extern void _gdb_out(const string& text);

// Update option menus
extern void update_options(bool noupd=false);

// Update option menus after toggle
extern void toggle_update_options();

// Update `Reset' button in preferences panel
extern void update_reset_preferences();

// Update argument buttons
extern void update_arg_buttons();

// Update user-defined buttons
extern void update_user_buttons();

// Update `Edit' menu (esp. Undo and Redo actions)
extern void update_edit_menus();

// Save current option state
extern void save_option_state();

#if defined(IF_XM)
// Last output position
extern XmTextPosition promptPosition;

// Last message position
extern XmTextPosition messagePosition;
#else
// Last output position
extern long promptPosition;

// Last message position
extern long messagePosition;
#endif

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
extern std::ostream *_dddlog;
#define dddlog (*_dddlog)
extern void init_dddlog();

// Text to ignore upon output
extern string gdb_out_ignore;

// Process next DDD event
extern void process_next_event();

// Process all pending DDD events
extern void process_pending_events();

#if defined(IF_XM)
// Setup is done
extern Boolean ddd_setup_done(XtPointer client_data);
#else
// Setup is done
extern bool ddd_setup_done(void);
#endif

// Register shells of menu ITEMS.
struct MMDesc;
extern void register_menu_shell(const MMDesc items[]);

// Process ^C key
extern bool process_emergencies();

#endif // _DDD_ddd_h
// DON'T ADD ANYTHING BEHIND THIS #endif
