// $Id$ -*- C++ -*-
// Xt TimeOut debugging routines

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char TimeOut_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "VarArray"
#endif

#include "TimeOut.h"
#include "strclass.h"
#include "assert.h"
#include "bool.h"

#include <iostream.h>
#include <stdlib.h>		// abort()

#undef XtAppAddTimeOut
#undef XtRemoveTimeOut

#define LOG_TIMERS 0

struct TimerInfo {
    XtIntervalId timer;		// The timer
    unsigned long interval;	// INTERVAL arg given to XtAppAddTimeOut()
    XtTimerCallbackProc proc;   // PROC     arg given to XtAppAddTimeOut()
    XtPointer closure;		// CLOSURE  arg given to XtAppAddTimeOut()
    string file;		// The file where XtAppAddTimeOut() was called
    int line;			// The line where XtAppAddTimeOut() was called
    TimerInfo *next;		// Next active timer
};

// List of active timers
static TimerInfo *active_timers = 0;

// When timer is called, remove TimerInfo from list
static void MyTimerProc(XtPointer client_data, XtIntervalId *id)
{
    TimerInfo *tm = (TimerInfo *)client_data;

    // Remove from active list
    bool removed = false;
    TimerInfo *tp = 0;
    for (TimerInfo *ti = active_timers; ti != 0; tp = ti, ti = ti->next)
    {
	if (ti == tm)
	{
	    if (tp != 0)
		tp->next      = ti->next;
	    else
		active_timers = ti->next;

	    removed = true;
	    break;
	}
    }

    assert (removed);

#if LOG_TIMERS
    clog << "TimeOut: " << tm->file << ":" << tm->line 
	 << ": timer " << tm->timer << " called\n";
#endif

    tm->proc(tm->closure, id);

    delete tm;
    return;
}

// Like XtAppAddTimeOut(), but also register timer in our list
XtIntervalId MyAppAddTimeOut(XtAppContext app_context,
			     unsigned long interval,
			     XtTimerCallbackProc proc,
			     XtPointer closure,
			     String file, int line)
{
    TimerInfo *ti = new TimerInfo;
    ti->timer    = XtAppAddTimeOut(app_context, interval, 
				   MyTimerProc, XtPointer(ti));
    ti->interval = interval;
    ti->proc     = proc;
    ti->closure  = closure;
    ti->file     = file ? "" : file;
    ti->line     = line;

    ti->next = active_timers;
    active_timers = ti;

#if LOG_TIMERS
    clog << "TimeOut: " 
	 << file << ":" << line << ": timer " << ti->timer << " added\n";
#endif

    return ti->timer;
}

// Like XtRemoveTimeOut(), but unregister TIMER from internal list. 
// Trigger an error if TIMER is not found (i.e. expired or removed)
extern void MyRemoveTimeOut(XtIntervalId timer, String file, int line)
{
    TimerInfo *tp = 0;
    for (TimerInfo *ti = active_timers; ti != 0; tp = ti, ti = ti->next)
    {
	if (timer == ti->timer)
	{
#if LOG_TIMERS
	    clog << "TimeOut: " << file << ":" << line
		 << ": timer " << ti->timer << " removed\n"
		 << "TimeOut: " << ti->file << ":" << ti->line
		 << ": this is the location where the timer was added.";
#endif

	    XtRemoveTimeOut(ti->timer);

	    if (tp != 0)
		tp->next      = ti->next;
	    else
		active_timers = ti->next;

	    delete ti;
	    return;
	}
    }

    // Timer not found - either already called or removed
    cerr << "TimeOut: " << file << ":" << line << ": timer " 
	 << timer << " expired\n";

    // `It is an error to remove a timer that has already gone off.'
    // (Asente/Swick, X WINDOW SYSTEM TOOLKIT, 6.4 `Timers').
    abort();
}
