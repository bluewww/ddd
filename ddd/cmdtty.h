// $Id$ -*- C++ -*-
// TTY command interface

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

#ifndef _DDD_cmdtty_h
#define _DDD_cmdtty_h

#include "bool.h"
#include "strclass.h"

// Initialize command tty
extern void init_command_tty();

// Close command tty
extern void kill_command_tty();

// Check if command tty is still running
extern bool tty_running();

// Output annotation TEXT when in annotation mode
extern void annotate(const char * text);

// Output TEXT on controlling TTY, unless we've just composed a command
extern void tty_out(const string& text);

// Output TEXT on controlling TTY (unconditionally)
extern void _tty_out(const string& text);

// Output TEXT on controlling TTY if we're in full_name_mode
extern void tty_full_name(const string& text);

// Give an artificial prompt
extern void prompt();

#endif // _DDD_cmdtty_h
// DON'T ADD ANYTHING BEHIND THIS #endif
