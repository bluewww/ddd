// $Id$ -*- C++ -*-
// Data Display progress meter

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ProgressMeter_h
#define _DDD_ProgressMeter_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "strclass.h"
#include "status.h"

#include <X11/Intrinsic.h>


//-----------------------------------------------------------------------------
// Show progress while creating or updating displays
//-----------------------------------------------------------------------------

class ProgressMeter {
public:
    int current;		// Current data to be processed
    int base;			// Data already processed
    int total;			// Total of data to be processed

    ProgressMeter(const string& msg);
    ~ProgressMeter();

private:
    string msg;			// The message shown
    StatusDelay delay;		// The delay shown
    int last_shown;		// Last shown amount
    bool (*old_background)(int); // DispValue bg proc
    bool aborted;		// True iff bg proc aborted

    // Update progess meter every UPDATE_THRESHOLD characters.
    static const int UPDATE_THRESHOLD;

    // Popup working dialog when updating from at least DIALOG_THRESHOLD chars.
    static const int DIALOG_THRESHOLD;

    static Widget dialog;
    static Widget scale;

    bool process(int remaining_length);

    static ProgressMeter *active; // Currently active object
    static bool _process(int remaining_length);

    static void CancelCB(Widget, XtPointer, XtPointer);

private:
    // No copy constructor
    ProgressMeter(const ProgressMeter &)
	: current(0), base(0), total(0), msg(),
	  delay(""), last_shown(0),
	  old_background(0),
	  aborted(false)
    {
	assert(0);
    }

    // No assignment
    ProgressMeter& operator = (const ProgressMeter &)
    {
	assert(0); return *this;
    }
};

#endif // _DDD_ProgressMeter_h
// DON'T ADD ANYTHING BEHIND THIS #endif
