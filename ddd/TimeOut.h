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

#ifndef _DDD_TimeOut_h
#define _DDD_TimeOut_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>

// Like XtAppAddTimeOut(), but also register timer in internal list.
extern XtIntervalId MyAppAddTimeOut(XtAppContext app_context,
				    unsigned long interval,
				    XtTimerCallbackProc proc,
				    XtPointer closure,
				    String file = 0,
				    int line = 0);

// Like XtRemoveTimeOut(), but unregister TIMER from internal list. 
// Trigger an error if TIMER is not found (i.e. expired or removed)
extern void MyRemoveTimeOut(XtIntervalId timer,
			    String file = 0,
			    int line = 0);

// Be sure not to intermix these!  A timer added with MyAppAddTimeOut()
// must be removed with MyRemoveTimeOut(); a timer added with
// XtAppAddTimeOut() must be removed with XtRemoveTimeOut().

// Unless NDEBUG is defined (as usually in optimization), use our
// replacement routines instead of the Xt functions.
#if !defined(NDEBUG)
#define XtAppAddTimeOut(a, i, p, c) \
    MyAppAddTimeOut((a), (i), (p), (c), __FILE__, __LINE__)
#define XtRemoveTimeOut(timer) \
    MyRemoveTimeOut((timer), __FILE__, __LINE__)
#endif // !defined(NDEBUG)

// Diagnostics

// Return number of pending timers
extern int MyPendingTimeOuts();

#endif // _DDD_TimeOut_h
// DON'T ADD ANYTHING BEHIND THIS #endif
