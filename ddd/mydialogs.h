// $Id$
// Create dialogs to select display expressions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

//-----------------------------------------------------------------------------
// Erzeugung von Motif-Dialogen zur Auswahl von Display-Ausdruecken
//-----------------------------------------------------------------------------

#ifndef _mydialogs_h
#define _mydialogs_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Motif-Zeugs
extern "C" {
#include <Xm/Xm.h>
}

// includes von Nora-Zeugs
#include "strclass.h"
#include "bool.h"

// Create a display selection list
Widget createDisplaySelectionList (Widget parent, String name,
				   ArgList args, Cardinal num_args);

// Get the display numbers
int *getDisplayNumbers(Widget selectionList);


// Set the elements of the display selection list
// label_list : Labels, using the format disp_nr ": " disp_name.
// selected   : Decides whether labels are to be selected
// list_length: Length of label_list[] and selected[]
//
void setLabelList (Widget  selectionList,
		   string  label_list[],
		   bool    selected[],
		   int     list_length);

// The default list charset
#define LIST_CHARSET "tt"

#endif
