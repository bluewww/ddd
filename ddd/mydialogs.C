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

char mydialogs_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Create dialogs to select display expressions
//-----------------------------------------------------------------------------

#include "mydialogs.h"

// System includes
#include <stdlib.h>

// Motif includes
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>

// Misc includes
#include "assert.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MString.h"

// Own includes
#include "string-fun.h"


// Local functions
static XmStringTable makeXmStringTable (string label_list[], int list_length,
					bool highlight_title);
static void freeXmStringTable (XmStringTable xmlabel_list, int list_length);


// Create a display selection list
Widget createDisplaySelectionList(Widget parent, String name,
				  ArgList args, Cardinal num_args)
{
    Widget selectionList = XmCreateScrolledList(parent, name, args, num_args);
    XtManageChild (selectionList);
    return selectionList;
}



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
		   bool    notify)
{
    XmStringTable xmlabel_list = 
	makeXmStringTable(label_list, list_length, highlight_title);

    XtVaSetValues (selectionList,
		   XmNitems,     xmlabel_list,
		   XmNitemCount, list_length,
		   NULL);

    XmListDeselectAllItems (selectionList);
    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmMULTIPLE_SELECT,
		   NULL);

    for (int i = 0; i < list_length; i++) 
	if (selected[i] == true) 
	    XmListSelectPos(selectionList, i + 1, Boolean(notify));

    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmEXTENDED_SELECT,
		   NULL);

    freeXmStringTable(xmlabel_list, list_length);
}


// Fill the display numbers in DISP_NRS
void getDisplayNumbers(Widget selectionList, IntArray& disp_nrs)
{
    static IntArray empty;
    disp_nrs = empty;

    XmStringTable selected_items;
    int selected_items_count;

    XtVaGetValues(selectionList,
		  XmNselectedItemCount, &selected_items_count,
		  XmNselectedItems, &selected_items,
		  NULL);

    for (int i = 0; i < selected_items_count; i++)
    {
	String _item;
	XmStringGetLtoR(selected_items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	if (has_nr(item))
	    disp_nrs += get_nr(item);
    }
}

// Create an array of XmStrings from the list LABEL_LIST of length
// LIST_LENGTH.  If HIGHLIGHT_TITLE is set, let the first line be bold.
static XmStringTable makeXmStringTable (string label_list[],
					int list_length, bool highlight_title)
{
    XmStringTable xmlist = 
	XmStringTable(XtMalloc(list_length * sizeof(XmString)));

    int i = 0;
    if (highlight_title && i < list_length)
    {
	xmlist[i] = XmStringCreateLtoR(label_list[i], LIST_TITLE_CHARSET);
	i++;
    }
	
    while (i < list_length)
    {
	xmlist[i] = XmStringCreateLtoR(label_list[i], LIST_CHARSET);
	i++;
    }

    return xmlist;
}

// Free the XmString table XMLIST of length LIST_LENGTH
static void freeXmStringTable (XmStringTable xmlist, int list_length)
{
    for (int i = 0; i < list_length; i++)
    {
	XmStringFree (*xmlist);
	xmlist++;
    }
}


// Select POS in LIST and make it visible
void ListSetAndSelectPos(Widget list, int pos)
{
    int top_item      = 0;
    int visible_items = 0;
    int items         = 0;
    XtVaGetValues(list,
		  XmNtopItemPosition, &top_item,
		  XmNvisibleItemCount, &visible_items,
		  XmNitemCount, &items,
		  NULL);

    XmListSelectPos(list, pos, False);

    if (pos == 1)
	XmListSetPos(list, pos);
    else if (pos == 0 || pos >= items)
	XmListSetBottomPos(list, 0);
    else if (pos - 1 < top_item)
	XmListSetPos(list, pos - 1);
    else if (pos + 1 >= top_item + visible_items)
	XmListSetBottomPos(list, pos + 1);
}
