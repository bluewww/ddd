// $Id$  -*- C++ -*-
// Scrolled Graph Editor

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_ScrolledGraphEdit_h
#define _DDD_ScrolledGraphEdit_h

#ifdef __GNUG__
#pragma interface
#endif

#include "GraphEdit.h"

// Declare specific GraphEdit class and instance datatypes
typedef struct _ScrolledGraphEditClassRec *ScrolledGraphEditWidgetClass;
typedef struct _ScrolledGraphEditRec      *ScrolledGraphEditWidget;

// Declare the class constant
extern WidgetClass scrolledGraphEditWidgetClass;

// Declare special access functions

// Create a graph editor with scrollbars; return the graph editor widget ID
Widget createScrolledGraphEdit(Widget parent, const _XtString name, ArgList arglist,
			       Cardinal argcount);

// For a given graph editor W, return its scroller
Widget scrollerOfGraphEdit(Widget w);

#endif // _DDD_ScrolledGraphEdit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
