// $Id$ -*- C++ -*-
// Panned Graph Editor

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

#ifndef _DDD_PannedGraphEdit_h
#define _DDD_PannedGraphEdit_h

#include "GraphEdit.h"

// Define any special resource names here that are not in <X11/StringDefs.h>

// Resource names

#define XtNminimumPannerWidth       "minimumPannerWidth"
#define XtNminimumPannerHeight      "minimumPannerHeight"
#define XtNmaximumScale             "maximumScale"


// Class types

#define XtCMinimumPannerWidth       "MinimumPannerWidth"
#define XtCMinimumPannerHeight      "MinimumPannerHeight"
#define XtCMaximumScale             "MaximumScale"


// Declare specific GraphEdit class and instance datatypes
typedef struct _PannedGraphEditClassRec *PannedGraphEditWidgetClass;
typedef struct _PannedGraphEditRec      *PannedGraphEditWidget;

// Declare the class constant
extern WidgetClass pannedGraphEditWidgetClass;

// Declare special access functions

// Create a graph editor with panner; return the graph editor widget ID
Widget createPannedGraphEdit(Widget parent, const _XtString name, ArgList arglist,
			     Cardinal argcount);

// For a given graph editor W, return its panner
Widget pannerOfGraphEdit(Widget w);

#endif // _DDD_PannedGraphEdit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
