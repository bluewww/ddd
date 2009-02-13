// $Id$ -*- C++ -*-
// Block X errors

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
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

char XErrorBlocker_rcsid[] = 
    "$Id$";

#include "XErrorB.h"
#include "assert.h"

// arnaud.desitter@nag.co.uk says `extern "C"' is required to fix
// warnings on Sun CC 5.x, which enforces a standard C++ rule where
// pointer to function are extern "C" or extern "C++" (see Sun C++
// migration guide).
extern "C" {
    static int _handler(Display *display, XErrorEvent *event)
    {
	return XErrorBlocker::handler(display, event);
    }
}

XErrorBlocker *XErrorBlocker::active = 0;

XErrorBlocker::XErrorBlocker(Display *display)
    // Initializing `_event' causes an error in GCC 2.8.1
    // Reported by "Derks, Victor" <Victor.Derks@nl2.vdogrp.de>
    : _display(display), _error_occurred(false), // _event(),
      saved_handler(0), saved_active(0)
{
    saved_handler = XSetErrorHandler(_handler);
    saved_active  = active;
    active = this;
}

XErrorBlocker::~XErrorBlocker()
{
    assert(active == this);

    sync();
    XSetErrorHandler(saved_handler);
    active = saved_active;
}

int XErrorBlocker::handler(Display *display, XErrorEvent *event)
{
    assert(active != 0);

    return active->handle(display, event);
}

int XErrorBlocker::handle(Display *, XErrorEvent *event)
{
    _error_occurred = true;
    _event = *event;

    return 0;
}
