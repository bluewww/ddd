// $Id$ -*- C++ -*-
// Interpret GDB output

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_status_h
#define _DDD_status_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "Delay.h"
#include "MString.h"
#include <X11/Intrinsic.h>

// Show MESSAGE in status window.  If FORCE is true, ensure that
// the entire message is visible.
void set_status(const string& message, bool force = false);

// Same, but use an MString.
void set_status_mstring(const MString& text, bool force = false);

// Return current contents of status line
extern const MString& current_status();

// Buttons and state
void set_buttons_from_gdb(Widget w, string& text);
void set_status_from_gdb(const string& text);
void set_selection_from_gdb(string& text);

 // True if last cmd came from GDB window
extern bool gdb_keyboard_command;

// True if the next line is to be displayed in the status line
extern bool show_next_line_in_status;

// This is convenient for setting the status during a function
class StatusDelay: public Delay {
private:
    string cause;

public:
    StatusDelay(const string& c):
	cause(c)
    {
	set_status(cause + "...");
    }

    ~StatusDelay()
    {
	set_status(cause + "... done.");
    }
};

#endif // _DDD_status_h
// DON'T ADD ANYTHING BEHIND THIS #endif
