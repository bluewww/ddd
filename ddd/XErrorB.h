// $Id$ -*- C++ -*-
// Block X errors

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_XErrorBlocker_h
#define _DDD_XErrorBlocker_h

extern "C" {
#include <X11/Intrinsic.h>
}
#include "bool.h"
#include "assert.h"
 
extern "C" {
    typedef int (*ddd_XErrorHandler) (Display *display, 
				      XErrorEvent *error_event);
}

class XErrorBlocker {
    Display *_display;
    bool _error_occurred;
    XErrorEvent _event;

    ddd_XErrorHandler saved_handler;
    XErrorBlocker *saved_active;

    static XErrorBlocker *active;

    XErrorBlocker(const XErrorBlocker&);

    XErrorBlocker& operator=(const XErrorBlocker&);

protected:
    virtual int handle(Display *, XErrorEvent *);
    virtual void sync() const { XSync(_display, False); }

public:
    XErrorBlocker(Display *display);
    virtual ~XErrorBlocker();

    // made public so that it can be wrapped by extern "C" function
    static int handler(Display *, XErrorEvent *);

    bool error_occurred() const
    {
	if (!_error_occurred)
	    sync();
	return _error_occurred;
    }

    const XErrorEvent& error_event() const
    {
	assert(_error_occurred);
	return _event;
    }
};

#endif // _DDD_XErrorBlocker_h
// DON'T ADD ANYTHING BEHIND THIS #endif
