// $Id$ -*- C++ -*-
// Panned Graph Editor

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

#ifndef _DDD_PannedGraphEdit_h
#define _DDD_PannedGraphEdit_h

#ifdef __GNUG__
#pragma interface
#endif

#include "GraphEdit.h"


// define any special resource names here that are not in <X11/StringDefs.h>

// Resource names

#define XtNminimumPannerWidth       "minimumPannerWidth"
#define XtNminimumPannerHeight      "minimumPannerHeight"
#define XtNmaximumScale             "maximumScale"


// Class types

#define XtCMinimumPannerWidth       "MinimumPannerWidth"
#define XtCMinimumPannerHeight      "MinimumPannerHeight"
#define XtCMaximumScale             "MaximumScale"


// declare specific GraphEdit class and instance datatypes
typedef struct _PannedGraphEditClassRec *PannedGraphEditWidgetClass;
typedef struct _PannedGraphEditRec      *PannedGraphEditWidget;

// declare the class constant
extern WidgetClass pannedGraphEditWidgetClass;

// declare special access functions

// create a graph editor with panner; return the graph editor widget ID
Widget createPannedGraphEdit(Widget parent, String name, ArgList arglist,
			     Cardinal argcount);

#endif // _DDD_PannedGraphEdit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
