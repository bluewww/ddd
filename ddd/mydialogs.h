// $Id$
// Create dialogs to select display expressions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

//-----------------------------------------------------------------------------
// Erzeugung von Motif-Dialogen zur Auswahl von Display-Ausdruecken
//-----------------------------------------------------------------------------

#ifndef _DDD_mydialogs_h
#define _DDD_mydialogs_h

#ifdef __GNUG__
#pragma interface
#endif

// Motif includes
#include <Xm/Xm.h>

// DDD includes
#include "strclass.h"
#include "bool.h"
#include "IntArray.h"
#include "charsets.h"

// Create a display selection list
Widget createDisplaySelectionList (Widget parent, String name,
				   ArgList args, Cardinal num_args);

// Get the display numbers
void getDisplayNumbers(Widget selectionList, IntArray& arr);


// Set the elements of the display selection list
// LABEL_LIST:      Labels, using the format disp_nr ": " disp_name.
// SELECTED:        Whether labels are to be selected
// LIST_LENGTH:     Length of label_list[] and selected[]
// HIGHLIGHT_TITLE: Whether the first line should be highlighted
// NOTIFY:          Whether callbacks should be invoked
//
void setLabelList (Widget  selectionList,
		   string  label_list[],
		   bool    selected[],
		   int     list_length,
		   bool    highlight_title,
		   bool    notify);

// The default list charset
#define LIST_CHARSET CHARSET_TT

// The default list title charset
#define LIST_TITLE_CHARSET CHARSET_TB

// Select POS in LIST and make it visible
void ListSetAndSelectPos(Widget list, int pos);

#endif // _DDD_mydialogs_h
// DON'T ADD ANYTHING BEHIND THIS #endif
