// $Id$
// DDD application resources

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _ddd_h
#define _ddd_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include <setjmp.h>

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
extern void _gdb_out(string text);

// Update option menus
extern void update_options();

// Last output position
extern XmTextPosition promptPosition;

// Last message position
extern XmTextPosition messagePosition;

// True if output is running
extern bool private_gdb_output;

// True if input is running
extern bool private_gdb_input;

// True if we have reached the main event loop
extern bool main_loop_entered;

// Longjmp environment to return to main event loop
extern jmp_buf main_loop_env;

// True if GDB issued initial prompt
extern bool gdb_initialized;

// Some more global objects
extern class GDBAgent   *gdb;
extern class DataDisp   *data_disp;
extern class SourceView *source_view;
extern class ArgField   *source_arg;

// Text to ignore upon output
extern string gdb_out_ignore;

// Pass the COMMAND given in CLIENT_DATA to gdb_command()
void gdbCommandCB(Widget w, XtPointer call_data, XtPointer client_data);

#endif
