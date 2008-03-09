// $Id$
// Create dialogs to select display expressions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

//-----------------------------------------------------------------------------
// Create dialogs to select display expressions
//-----------------------------------------------------------------------------

#ifndef _DDD_mydialogs_h
#define _DDD_mydialogs_h

// Motif includes
#include <Xm/Xm.h>

// DDD includes
#include "strclass.h"
#include "bool.h"
#include "IntArray.h"
#include "charsets.h"

// Create a selection box with a top-level shell.  This is like
// XmCreateSelectionDialog, but the parent is a top-level shell.
Widget createTopLevelSelectionDialog(Widget parent, const _XtString name,
				     ArgList args, Cardinal num_args);

// Get the item numbers
void getItemNumbers(Widget selectionList, IntArray& arr);


// Set the elements of the display selection list
// LABEL_LIST:      Labels, using the format disp_nr ": " disp_name.
// SELECTED:        Whether labels are to be selected
// LIST_LENGTH:     Length of label_list[] and selected[]
// HIGHLIGHT_TITLE: Whether the first line should be highlighted
// NOTIFY:          Whether callbacks should be invoked
//
void setLabelList (Widget  selectionList,
		   const string  label_list[],
		   const bool selected[],
		   int     list_length,
		   bool    highlight_title,
		   bool    notify);

// Replace all elements in SELECTIONLIST with the corresponding
// entries in LABEL_LIST (i.e. with the same leading number).
void updateLabelList (Widget  selectionList,
		      const string  label_list[],
		      int     list_length);

// The default list charset
#define LIST_CHARSET CHARSET_TT

// The default list title charset
#define LIST_TITLE_CHARSET CHARSET_TB

// Select POS in LIST and make it visible
void ListSetAndSelectPos(Widget list, int pos);

// Create an array of XmStrings from the list LABEL_LIST of length
// LIST_LENGTH.  If HIGHLIGHT_TITLE is set, let the first line be bold.
XmStringTable makeXmStringTable(const string label_list[],
				int list_length, 
				bool highlight_title = false);

// Free the XmString table XMLIST of length LIST_LENGTH
void freeXmStringTable(XmStringTable xmlist, int list_length);


#endif // _DDD_mydialogs_h
// DON'T ADD ANYTHING BEHIND THIS #endif
