// $Id$  -*- C++ -*-
// Scrolled Graph Editor (private part)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef __ScrolledGraphEditPrivate_h__
#define __ScrolledGraphEditPrivate_h__

#ifdef __GNUG__
#pragma interface
#endif

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

#endif // __ScrolledGraphEditPrivate_h__
// DON'T ADD ANYTHING BEHIND THIS #endif
