// $Id$ -*- C++ -*-
// Interpret GDB output

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

#ifndef _DDD_status_h
#define _DDD_status_h

#include "strclass.h"
#include "bool.h"
#include "Delay.h"
#include "MString.h"
#include <X11/Intrinsic.h>

// Show MESSAGE in status window.
// If TEMPORARY is set, override locks and do not add to status history.
void set_status(const string& message, bool temporary = false);

// Same, but use an MString.
void set_status_mstring(const MString& text, bool temporary = false);

// Return current contents of status line
extern const MString& current_status(void);

// Status history
extern Widget status_history(Widget parent);

// Buttons and state
void set_buttons_from_gdb(Widget w, string& text);
void set_status_from_gdb(const string& text);

 // True if last cmd came from GDB window
extern bool gdb_keyboard_command;

// True if asking `yes or no'
extern bool gdb_asks_yn;

// True if the next line is to be displayed in the status line
extern bool show_next_line_in_status;

// Number of messages to keep in status history
extern int status_history_size;

// Status lock
void lock_status(void);		// Place a lock on status
void unlock_status(void);		// Remove lock from status
void reset_status_lock(void);	// Remove all locks from status

// These are convenient for setting the status during a function
class _StatusMsg {
private:
    string cause;

public:
    string outcome;

    _StatusMsg(const string& c)
	: cause(c), outcome("done")
    {
	set_status(cause + "...");
    }

    _StatusMsg(const char *c)
	: cause(c), outcome("done")
    {
	set_status(cause + "...");
    }

    virtual ~_StatusMsg()
    {
	set_status(cause + "..." + outcome + ".");
    }
};

class StatusMsg: public _StatusMsg {
public:
    StatusMsg(const string& c)
	: _StatusMsg(c)
    {
	lock_status();
    }
    StatusMsg(const char *c)
	: _StatusMsg(c)
    {
	lock_status();
    }
    virtual ~StatusMsg()
    {
	unlock_status();
    }
};

class StatusDelay: public StatusMsg, public Delay {
public:
    StatusDelay(const string& c)
	: StatusMsg(c)
    {}
    StatusDelay(const char *c)
	: StatusMsg(c)
    {}
};

#endif // _DDD_status_h
// DON'T ADD ANYTHING BEHIND THIS #endif
