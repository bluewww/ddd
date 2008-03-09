// $Id$ -*- C++ -*-
// Xt TimeOut debugging routines

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_TimeOut_h
#define _DDD_TimeOut_h

#include <X11/Intrinsic.h>

// Like XtAppAddTimeOut(), but also register timer in internal list.
extern XtIntervalId MyAppAddTimeOut(XtAppContext app_context,
				    unsigned long interval,
				    XtTimerCallbackProc proc,
				    XtPointer closure,
				    const char *file = 0,
				    int line = 0);

// Like XtRemoveTimeOut(), but unregister TIMER from internal list. 
// Trigger an error if TIMER is not found (i.e. expired or removed)
extern void MyRemoveTimeOut(XtIntervalId timer,
			    const char *file = 0,
			    int line = 0);

// Be sure not to intermix these!  A timer added with MyAppAddTimeOut()
// must be removed with MyRemoveTimeOut(); a timer added with
// XtAppAddTimeOut() must be removed with XtRemoveTimeOut().

// Unless NDEBUG is defined (as usually in optimization), use our
// replacement routines instead of the Xt functions.
#if !defined(NDEBUG)
#define XtAppAddTimeOut(a, i, p, c) \
    MyAppAddTimeOut((a), (i), (p), (c), (const char *)__FILE__, __LINE__)
#define XtRemoveTimeOut(timer) \
    MyRemoveTimeOut((timer), (const char *)__FILE__, __LINE__)
#endif // !defined(NDEBUG)

// Diagnostics

// Return number of pending timers
extern int MyPendingTimeOuts();

#endif // _DDD_TimeOut_h
// DON'T ADD ANYTHING BEHIND THIS #endif
