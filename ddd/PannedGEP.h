// $Id$  -*- C++ -*-
// Panned Graph Editor (private part)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

#ifndef _DDD_PannedGraphEditPrivate_h
#define _DDD_PannedGraphEditPrivate_h

#ifdef __GNUG__
#pragma interface
#endif

#include <Xm/Xm.h>

// Superclasses
#define new new_w
#define class class_w
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/Xaw/PortholeP.h>
#undef new
#undef class

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

#endif // _DDD_PannedGraphEditPrivate_h
// DON'T ADD ANYTHING BEHIND THIS #endif
