// $Id$  -*- C++ -*-
// Scrolled Graph Editor (private part)

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef __ScrolledGraphEditPrivate_h__
#define __ScrolledGraphEditPrivate_h__

#ifdef __GNUG__
#pragma interface
#endif

#include <Xm/Xm.h>

// Superclasses
extern "C" {
#define new new_w
#define class class_w
#include <Xm/XmP.h>
#include <X11/CoreP.h>
#include <Xm/ScrolledWP.h>
#undef new
#undef class
}

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
