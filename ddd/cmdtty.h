// $Id$ -*- C++ -*-
// TTY command interface

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_cmdtty_h
#define _DDD_cmdtty_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "strclass.h"

// Initialize command tty
extern void init_command_tty();

// Check if command tty is still running
extern bool tty_running();

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
