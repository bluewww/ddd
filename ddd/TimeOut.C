// $Id$ -*- C++ -*-
// Xt TimeOut debugging routines

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char TimeOut_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "TimeOut.h"
#include "strclass.h"
#include "assert.h"
#include "bool.h"

#include <iostream.h>
#include <stdlib.h>		// abort()

#undef XtAppAddTimeOut
#undef XtRemoveTimeOut

// Set this to 1 to log all timer actions
#define LOG_TIMERS 0

struct TimerInfo {
    XtIntervalId timer;		// The id as received from XtAppAddTimeOut()
    XtIntervalId tic;		// The unique id we use instead
    unsigned long interval;	// INTERVAL arg given to XtAppAddTimeOut()
    XtTimerCallbackProc proc;   // PROC     arg given to XtAppAddTimeOut()
    XtPointer closure;		// CLOSURE  arg given to XtAppAddTimeOut()
    string file;		// The file where XtAppAddTimeOut() was called
    int line;			// The line where XtAppAddTimeOut() was called
    TimerInfo *next;		// Next pending timer

    TimerInfo()
	: timer(0), tic(0), interval(0), proc(0), closure(0), file(), line(0),
	  next(0)
    {}

private:
    TimerInfo(const TimerInfo&)
	: timer(0), tic(0), interval(0), proc(0), closure(0), file(), line(0),
	  next(0)
    {
	assert(0);
    }

    TimerInfo& operator = (const TimerInfo&)
    {
	assert(0); return *this;
    }
};

// List of pending timers
static TimerInfo *pending_timers = 0;

// Counter
static XtIntervalId timer_tics = 0;

// Return number of pending timers (i.e. not yet called)
int MyPendingTimeOuts()
{
    int count = 0;
    for (TimerInfo *ti = pending_timers; ti != 0; ti = ti->next)
	count++;

    return count;
}

// When timer is called, remove TimerInfo from list
static void MyTimerProc(XtPointer client_data, XtIntervalId *)
{
    TimerInfo *tm = (TimerInfo *)client_data;

    // Remove from pending list
    bool removed = false;
    TimerInfo *tp = 0;
    for (TimerInfo *ti = pending_timers; ti != 0; tp = ti, ti = ti->next)
    {
	if (ti == tm)
	{
	    if (tp != 0)
		tp->next = ti->next;
	    else
		pending_timers = ti->next;

	    removed = true;
	    break;
	}
    }

    assert(removed);
    if (!removed)		// Just use it
	abort();

#if LOG_TIMERS
    clog << "TimeOut: " << tm->file << ":" << tm->line << ": timer " 
	 << tm->tic << " (" << XtPointer(tm->timer) << ")"
	 << " expired (" << MyPendingTimeOuts() << " still pending)\n";
#endif

    tm->proc(tm->closure, &tm->tic);

    delete tm;
    return;
}

// Like XtAppAddTimeOut(), but also register timer in internal list.
XtIntervalId MyAppAddTimeOut(XtAppContext app_context,
			     unsigned long interval,
			     XtTimerCallbackProc proc,
			     XtPointer closure,
			     String file, int line)
{
    TimerInfo *tm = new TimerInfo;
    tm->timer    = XtAppAddTimeOut(app_context, interval, 
				   MyTimerProc, XtPointer(tm));
    tm->interval = interval;
    tm->proc     = proc;
    tm->closure  = closure;
    tm->file     = (file ? file : "");
    tm->line     = line;
    tm->tic      = ++timer_tics;

    tm->next = pending_timers;
    pending_timers = tm;

#if LOG_TIMERS
    clog << "TimeOut: " 
	 << file << ":" << line << ": timer " 
	 << tm->tic << " (" << XtPointer(tm->timer) << ") added "
	 << "(" << MyPendingTimeOuts() << " still pending)\n";
#endif

    // Note: we return a private XtIntervalId instead of the one
    // returned by XtAppAddInput.  This gives every timer a unique id
    // and thus enables us to check for expired timers.
    return tm->tic;
}

// Like XtRemoveTimeOut(), but unregister TIMER from internal list. 
// Trigger an error if TIMER is not found (i.e. expired or removed)
extern void MyRemoveTimeOut(XtIntervalId tic, String file, int line)
{
    TimerInfo *tp = 0;
    for (TimerInfo *ti = pending_timers; ti != 0; tp = ti, ti = ti->next)
    {
	if (tic == ti->tic)
	{
#if LOG_TIMERS
	    clog << "TimeOut: " << file << ":" << line << ": timer "
		 << ti->tic << " (" << XtPointer(ti->timer) << ") removed "
		 << "(" << MyPendingTimeOuts() << " still pending)\n"
		 << "TimeOut: " << ti->file << ":" << ti->line
		 << ": this is the location where the timer was added.\n";
#endif

	    XtRemoveTimeOut(ti->timer);

	    if (tp != 0)
		tp->next = ti->next;
	    else
		pending_timers = ti->next;

	    delete ti;
	    return;
	}
    }

    // Timer not found - either already called or removed
    cerr << "TimeOut: " << file << ":" << line << ": timer " 
	 << tic << " expired\n";

    // `It is an error to remove a timer that has already gone off.'
    // (Asente/Swick, X WINDOW SYSTEM TOOLKIT, 6.4 `Timers').
    abort();
}
