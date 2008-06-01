// $Id$  -*- C++ -*-
// Scrolled Graph Editor (private part)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ScrolledGraphEditPrivate_h
#define _DDD_ScrolledGraphEditPrivate_h

#include <Xm/Xm.h>

// Superclasses
#define new new_w
#define class class_w
#include <Xm/XmP.h>
#include <X11/CoreP.h>
#include <Xm/ScrolledWP.h>
#undef new
#undef class

#ifdef MAX
#undef MAX			// <Xm/XmP.h> on HP-UX defines this
#endif

// This class
#include "ScrolledGE.h"


// The GraphEdit class record

typedef struct {
    XtPointer extension;
} ScrolledGraphEditClassPart;

typedef struct _GraphEditClassRec {
    CoreClassPart              core_class;
    CompositeClassPart         composite_class;
    ConstraintClassPart        constraint_class;
    XmManagerClassPart         manager_class;
    XmScrolledWindowClassPart  swindow_class;
    ScrolledGraphEditClassPart sge_class;
} ScrolledGraphEditClassRec;

extern ScrolledGraphEditClassRec scrolledGraphEditClassRec;


// The ScrolledGraphEdit instance record

typedef struct _ScrolledGraphEditPart {
    // resources
    int mumble;
} ScrolledGraphEditPart;

typedef struct _ScrolledGraphEditRec {
    CorePart              core;
    CompositePart         composite;
    ConstraintPart        constraint;
    XmManagerPart         manager;
    XmScrolledWindowPart  swindow;
    ScrolledGraphEditPart sge;
} ScrolledGraphEditRec;

#endif // _DDD_ScrolledGraphEditPrivate_h
// DON'T ADD ANYTHING BEHIND THIS #endif
