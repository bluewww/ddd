// $Id$  -*- C++ -*-
// Panned Graph Editor (private part)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

// $Log$
// Revision 1.1  1995/05/01 15:47:10  zeller
// Initial revision
//
// Revision 1.2  1995/04/14  15:26:08  zeller
// New: have minimum/maximum size values for panner
//
// Revision 1.1  1995/04/14  14:07:49  zeller
// Initial revision
//

#ifndef __PannedGraphEditPrivate_h__
#define __PannedGraphEditPrivate_h__

#ifdef __GNUG__
#pragma interface
#endif

#include <Xm/Xm.h>

// Superclasses
extern "C" {
#define new new_w
#define class class_w
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/Xaw/PortholeP.h>
#undef new
#undef class
}

// This class
#include "PannedGE.h"


// The GraphEdit class record

typedef struct {
    XtPointer extension;
} PannedGraphEditClassPart;

typedef struct _GraphEditClassRec {
    CoreClassPart              core_class;
    CompositeClassPart         composite_class;
    PortholeClassPart          porthole_class;
    PannedGraphEditClassPart   pannedGraphEdit_class;
} PannedGraphEditClassRec;

extern PannedGraphEditClassRec pannedGraphEditClassRec;


// The PannedGraphEdit instance record

typedef struct _PannedGraphEditPart {
    // resources
    Dimension minimumPannerWidth;
    Dimension minimumPannerHeight;
    Dimension maximumScale;
} PannedGraphEditPart;

typedef struct _PannedGraphEditRec {
    CorePart              core;
    CompositePart         composite;
    PortholePart          porthole;
    PannedGraphEditPart   pannedGraphEdit;
} PannedGraphEditRec;

#endif // __PannedGraphEditPrivate_h__
// DON'T ADD ANYTHING BEHIND THIS #endif
