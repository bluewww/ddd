// $Id$ -*- C++ -*-
// Block X errors

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

char XErrorBlocker_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "XErrorB.h"
#include "assert.h"

XErrorBlocker *XErrorBlocker::active = 0;

XErrorBlocker::XErrorBlocker(Display *display)
    : _display(display), _error_occurred(false),
      saved_handler(0), saved_active(0)
{
    saved_handler = XSetErrorHandler(handler);
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
