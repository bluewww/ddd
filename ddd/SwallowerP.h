// $Id$ -*- C++ -*-
// Swallower Widget - swallow a user application

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

#ifndef _DDD_SwallowerPrivate_h
#define _DDD_SwallowerPrivate_h

// This class
#include "Swallower.h"

#include <X11/IntrinsicP.h>

// Superclasses
extern "C" {
#define new new_w
#define class class_w
#include <X11/CoreP.h>
#undef new
#undef class
}

// The Swallower class record

typedef struct _SwallowerClassPart {
    XtPointer extension;
} SwallowerClassPart;

typedef struct _SwallowerClassRec {
    CoreClassPart        core_class;
    SwallowerClassPart   swallower_class;
} SwallowerClassRec;

extern SwallowerClassRec swallowerClassRec;

typedef struct _SwallowerPart {
    // Resources
    Window window;		// Window of swallowed application

    // Callbacks
    XtCallbackList windowGoneProc;      // Window has gone
    XtCallbackList windowCreatedProc;   // Window has been created

    // Private state
    SwallowerWidget next;	// Next swallower
} SwallowerPart;

typedef struct _SwallowerRec {
    CorePart        core;
    SwallowerPart   swallower;
} SwallowerRec;

#endif // _DDD_SwallowerPrivate_h
// DON'T ADD ANYTHING BEHIND THIS #endif
