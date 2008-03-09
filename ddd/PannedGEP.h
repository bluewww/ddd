// $Id$  -*- C++ -*-
// Panned Graph Editor (private part)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
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

#ifndef _DDD_PannedGraphEditPrivate_h
#define _DDD_PannedGraphEditPrivate_h

#include <Xm/Xm.h>

// Superclasses
#define new new_w
#define class class_w
extern "C" {
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/Xaw/PortholeP.h>
}
#undef new
#undef class

// This class
#include "PannedGE.h"


// The GraphEdit class record

typedef struct {
    XtPointer extension;
} PannedGraphEditClassPart;

typedef struct _PannedGraphEditClassRec {
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

#endif // _DDD_PannedGraphEditPrivate_h
// DON'T ADD ANYTHING BEHIND THIS #endif
